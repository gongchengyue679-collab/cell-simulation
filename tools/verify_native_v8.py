"""End-to-end V8 assays: single founder, actual bud sizes, reproducibility and ledgers."""
from __future__ import annotations

import argparse
import concurrent.futures
import json
from pathlib import Path
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / 'output/release-v8/细胞模拟V8.exe'
OUT = ROOT / 'output/verification/native-v8'
BASE = json.loads((ROOT / 'configs/v8.json').read_text(encoding='utf-8'))


def run(seed: int, steps: int, name: str, *, config=None, resume=None):
    folder = OUT / name
    folder.mkdir(parents=True, exist_ok=True)
    cfg = dict(BASE if config is None else config)
    cfg.update(random_seed=seed, generation_time_limit=0.04 if cfg['simulation_version'] == 'V8' else 0.0)
    path = folder / 'config.json'
    path.write_text(json.dumps(cfg), encoding='utf-8')
    state = folder / 'state.json'
    command = [str(EXE), '--headless', '--continuous', '--steps', str(steps), '--save', str(state), '--output', str(folder / 'run')]
    command += ['--resume', str(resume)] if resume else ['--config', str(path)]
    started = time.perf_counter()
    subprocess.run(command, cwd=ROOT, timeout=600, check=True)
    statistics = json.loads((folder / 'run/statistics.json').read_text(encoding='utf-8'))
    saved = json.loads(state.read_text(encoding='utf-8'))
    return statistics, saved, time.perf_counter() - started


def assay(seed: int, steps: int):
    stats, state, elapsed = run(seed, steps, f'seed-{seed}')
    sizes = [e['cells'] for e in stats['bud_events']]
    error = max(abs(stats[k]) for k in ('sugar_balance_error', 'waste_balance_error', 'biomass_balance_error'))
    tiny = sum(n for n in stats['component_sizes'] if n < 8) / max(1, stats['cells'])
    passed = bool(sizes and min(sizes) >= BASE['fragment_min_cells'] and stats['cells'] >= 100 and stats['small_fragment_fraction'] <= 0.20 and tiny <= 0.05 and error < 1e-7 and state['generation'] == 0)
    result = dict(seed=seed, passed=passed, birth_sizes=sizes, living_cells=stats['cells'], colonies=stats['active_colonies'], physical_components=stats['component_sizes'], small_fragment_fraction=stats['small_fragment_fraction'], tiny_fragment_fraction=tiny, balance_error=error, seconds=elapsed)
    print(json.dumps(result), flush=True)
    return result


def checks(steps: int):
    expected = json.loads((OUT / 'seed-0/state.json').read_text(encoding='utf-8'))
    _, repeated, _ = run(0, steps, 'repeat-seed-0')
    assert repeated == expected, 'same seed must reproduce the entire signal/lineage/RNG state'
    midpoint = steps // 2
    run(0, midpoint, 'split-first')
    _, resumed, _ = run(0, steps - midpoint, 'split-resumed', resume=OUT / 'split-first/state.json')
    assert resumed == expected, 'resume must preserve local communication and physical lineage exactly'
    lethal = dict(BASE)
    lethal.update(fragmentation_enabled=False, light_intensity=0, photosynthesis_rate=0, base_cost=1, death_damage_threshold=.2, starvation_damage_rate=100)
    _, dead, _ = run(0, 500, 'extinction-only', config=lethal)
    assert dead['generation'] > 0 and dead['lastTransition'] == '全部细胞死亡'
    print('PASS deterministic replay, exact continuation and extinction-only restart', flush=True)
    legacy = ROOT / 'output/release-cpp/细胞模拟V6.exe'
    if legacy.exists():
        cfg = dict(BASE)
        cfg.update(simulation_version='V6', fragmentation_enabled=False)
        _, current, _ = run(0, 100, 'legacy-current', config=cfg)
        folder = OUT / 'legacy-reference'
        folder.mkdir(parents=True, exist_ok=True)
        saved = folder / 'state.json'
        subprocess.run([str(legacy), '--headless', '--continuous', '--steps', '100', '--config', str(OUT / 'legacy-current/config.json'), '--save', str(saved), '--output', str(folder / 'run')], cwd=ROOT, timeout=60, check=True)
        reference = json.loads(saved.read_text(encoding='utf-8'))['simulation']
        actual = current['simulation']
        assert len(actual['cells']) == len(reference['cells'])
        error = 0.0
        for a, b in zip(actual['cells'], reference['cells']):
            for key in ('x', 'y', 'r', 'mass', 'sugar', 'waste', 'damage', 'px', 'py', 'photo', 'ts', 'tw', 'ms', 'mw', 'division'):
                error = max(error, abs(a[key] - b[key]))
        for key in ('sugarField', 'wasteField', 'lightField'):
            error = max(error, max(abs(a - b) for a, b in zip(actual[key], reference[key])))
        assert error < 1e-8, error
        print(f'PASS V6 source regression against preserved executable (max error {error:.3g})', flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--seeds', type=int, nargs='+', default=list(range(10)))
    parser.add_argument('--steps', type=int, default=7500)
    parser.add_argument('--checks', action='store_true')
    args = parser.parse_args()
    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        results = list(pool.map(lambda seed: assay(seed, args.steps), args.seeds))
    summary = dict(steps=args.steps, seeds=args.seeds, passed=sum(r['passed'] for r in results), results=results)
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    assert summary['passed'] >= (8 if len(results) == 10 else len(results)), summary
    if args.checks:
        checks(args.steps)
