#include "../native/presets.hpp"
#include "../native/state.hpp"
#include <iostream>
#include <stdexcept>
using namespace cs;
static void require(bool ok,const char*message){if(!ok)throw std::runtime_error(message);}
static Config config(){auto j=v8_preset();j["bud_initiation_rate"]=0.;return Config::from(j);}
static Simulation chain(){Simulation s(config());s.c.clear();for(int i=0;i<8;i++){Cell v;v.id=i;v.x=10+.85*i;v.y=10;v.sugar=1;s.c.push_back(v);}s.c.back().x+=5;s.updateContacts();s.initialS=s.total();s.initialMass=s.c.size();return s;}
static void relayTest(){
 auto s=chain();s.c[0].signals[0]=1;s.propagateWiredSignals();
 require(s.c[1].signals[0]>0,"contact neighbor must receive the signal");
 require(s.c[2].signals[0]==0,"a signal must not jump two contacts in one step");
 require(s.c.back().signals[0]==0,"disconnected cells must not receive a wired signal");
 s.propagateWiredSignals();require(s.c[2].signals[0]>0,"a second contact must receive the relayed signal next step");
 require(s.regulationSpent>0,"communication must consume sugar");
 require(std::abs(s.total()-(s.initialS-s.regulationSpent))<1e-12,"signal energy must enter the sugar ledger");
 require(std::abs(s.total(true)-s.wasteProduced)<1e-12,"signal waste must enter the waste ledger");
 auto t=chain();t.c[0].budDomain=1;t.c[0].budQuorum=1;t.nextBudId=2;t.propagateCooperativeBuds();
 require(t.c[1].budDomain==1&&t.c[2].budDomain==0,"bud membership must also propagate one contact per step");
 t.propagateCooperativeBuds();require(t.c[2].budDomain==1,"the bud signal must continue along contacts");
 std::cout<<"PASS local relays, disconnected isolation, communication cost and conservation\n";
}
static double quorum(int rings){auto cfg=config();cfg.single_cell_start=false;cfg.tissue_rings=rings;cfg.world_size=60;Simulation s(cfg);double right=0;for(auto&v:s.c){v.budDomain=1;v.budQuorum=1;v.sugar=10;right=std::max(right,v.x);}Cell parent;parent.id=9999;parent.x=right+.85;parent.y=30;parent.sugar=10;parent.budRest=1000;s.c.push_back(parent);s.updateContacts();for(int i=0;i<1100;i++)s.propagateCooperativeBuds();double maximum=0;for(auto&v:s.c)maximum=std::max(maximum,v.budCommit);return maximum;}
static void quorumTest(){double small=quorum(1),large=quorum(6);std::cout<<"quorum small="<<small<<" large="<<large<<"\n";require(small<.45,"seven cells must not authorize a physical bud boundary");require(large>.8,"a sustained large contact patch must support collective commitment");}
static void metadataTest(){auto a=chain();auto b=readState(stateJson(a));for(auto&v:b->c){v.colonyId=100+v.id;v.parentColony=999;v.colonyGeneration=27;}a.mechanics(a.cfg.dt);b->mechanics(b->cfg.dt);for(size_t i=0;i<a.c.size();i++){require(a.c[i].x==b->c[i].x&&a.c[i].y==b->c[i].y&&a.c[i].vx==b->c[i].vx&&a.c[i].vy==b->c[i].vy,"lineage labels must never control mechanical forces");}std::cout<<"PASS colony metadata does not control mechanics\n";}
static void physicalTest(){
 auto cfg=config();cfg.single_cell_start=false;cfg.tissue_rings=8;cfg.world_size=60;cfg.motility_rate=0;
 Simulation s(cfg);int seed=0;for(int i=0;i<int(s.c.size());i++){s.c[i].sugar=10;if(s.c[i].x>s.c[seed].x)seed=i;}
 s.initialS=s.total();s.c[seed].budDomain=1;s.c[seed].budQuorum=1;s.c[seed].budX=1;s.c[seed].budY=0;s.c[seed].budGenome[0]=1.1;s.c[seed].budMutationLoci=1;s.nextBudId=2;
 auto noRegistration=readState(stateJson(s));noRegistration->cfg.fragment_min_cells=1000;
 for(int i=0;i<2200;i++){
  for(auto*sim:{&s,noRegistration.get()}){sim->updateContacts();sim->propagateCooperativeBuds();sim->mechanics(cfg.dt);sim->recordCooperativeBirths();sim->steps++;sim->time=sim->steps*cfg.dt;}
  for(size_t k=0;k<s.c.size();k++){auto&a=s.c[k];auto&b=noRegistration->c[k];require(a.x==b.x&&a.y==b.y&&a.vx==b.vx&&a.vy==b.vy&&a.genome==b.genome&&a.budCommit==b.budCommit,"registration thresholds and lineage events must not affect physics or genetics");}
  if(i%500==0){auto st=s.statistics();std::cout<<"physical t="<<s.time<<" tagged="<<st["bud_tagged_cells"].num()<<" committed="<<st["bud_committed_cells"].num()<<" components="<<st["component_sizes"].dump()<<"\n";}
 }
 std::cout<<"physical births="<<s.fragmentBirths<<" events="<<s.budEvents.dump()<<"\n";
 require(s.fragmentBirths>0,"a contact-relayed patch must detach physically as a large daughter");
 require(noRegistration->fragmentBirths==0,"the control scanner should not label any daughter");
 for(auto&v:s.c)require(v.genome[0]==(v.budGeneApplied?1.1:1.),"only cells receiving and committing the genetic relay may change their genes");
 for(auto&e:s.budEvents.a)require(e["cells"].num()>=48,"no small physical fragment may count as a daughter");
 require(std::abs(s.total()-(s.initialS-s.regulationSpent-s.pumpSpent))<1e-8,"budding and motion must conserve the sugar ledger");
 require(std::abs(s.total(true)-s.wasteProduced)<1e-8,"budding and motion waste must be conserved");
 auto restored=readState(stateJson(s));require(stateJson(s).dump()==stateJson(*restored).dump(),"local bud signals, timers, events and RNG must round-trip exactly");
 std::cout<<"PASS physical large-bud release, material ledgers and exact state restoration\n";
}
int main(){try{relayTest();quorumTest();metadataTest();physicalTest();return 0;}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}}
