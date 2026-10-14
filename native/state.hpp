#pragma once
#include "simulation.hpp"
namespace cs {
inline Json cellJson(const Cell& v){Json j=Json::object();
j["id"] = double(v.id);
j["parent"] = double(v.parent);
j["status"] = double(v.status);
j["colonyId"] = double(v.colonyId);
j["parentColony"] = double(v.parentColony);
j["colonyGeneration"] = double(v.colonyGeneration);
j["x"] = (v.x);
j["y"] = (v.y);
j["r"] = (v.r);
j["mass"] = (v.mass);
j["age"] = (v.age);
j["sugar"] = (v.sugar);
j["waste"] = (v.waste);
j["photo"] = (v.photo);
j["ts"] = (v.ts);
j["tw"] = (v.tw);
j["ms"] = (v.ms);
j["mw"] = (v.mw);
j["res"] = (v.res);
j["division"] = (v.division);
j["lock"] = (v.lock);
j["damage"] = (v.damage);
j["starve"] = (v.starve);
j["toxic"] = (v.toxic);
j["activity"] = (v.activity);
j["px"] = (v.px);
j["py"] = (v.py);
j["vx"] = (v.vx);
j["vy"] = (v.vy);
j["exposure"] = (v.exposure);
j["pressure"] = (v.pressure);
j["aspect"] = (v.aspect);
j["development"] = (v.development);
j["light"] = (v.light);
j["remaining"] = (v.remaining);
j["fragmentReady"] = (v.fragmentReady);
j["budDomain"]=double(v.budDomain);
j["budHop"]=double(v.budHop);
j["budAge"]=double(v.budAge);
j["budQuorum"]=double(v.budQuorum);
j["budCommit"]=double(v.budCommit);
j["budRest"]=double(v.budRest);
j["budX"]=double(v.budX);
j["budY"]=double(v.budY);
j["budLink"]=v.budLink;j["budLostTime"]=v.budLostTime;
j["budMutationLoci"]=v.budMutationLoci;j["budGeneApplied"]=v.budGeneApplied;j["budGenome"]=Json::array();for(auto a:v.budGenome)j["budGenome"].push(a);

j["alive"] = (v.alive);
j["maintenance"] = (v.maintenance);
j["genome"]=Json::array();for(auto a:v.genome)j["genome"].push(a);
j["rates"]=Json::array();for(auto a:v.rates)j["rates"].push(a);
j["observed"]=Json::array();for(auto a:v.observed)j["observed"].push(a);
j["signals"]=Json::array();for(auto a:v.signals)j["signals"].push(a);
return j;}
inline Cell readCell(const Json& j){Cell v;
if(j.has("id"))v.id=decltype(v.id)(j["id"].num());
if(j.has("parent"))v.parent=decltype(v.parent)(j["parent"].num());
if(j.has("status"))v.status=decltype(v.status)(j["status"].num());
if(j.has("colonyId"))v.colonyId=decltype(v.colonyId)(j["colonyId"].num());
if(j.has("parentColony"))v.parentColony=decltype(v.parentColony)(j["parentColony"].num());
if(j.has("colonyGeneration"))v.colonyGeneration=decltype(v.colonyGeneration)(j["colonyGeneration"].num());
if(j.has("x"))v.x=decltype(v.x)(j["x"].num());
if(j.has("y"))v.y=decltype(v.y)(j["y"].num());
if(j.has("r"))v.r=decltype(v.r)(j["r"].num());
if(j.has("mass"))v.mass=decltype(v.mass)(j["mass"].num());
if(j.has("age"))v.age=decltype(v.age)(j["age"].num());
if(j.has("sugar"))v.sugar=decltype(v.sugar)(j["sugar"].num());
if(j.has("waste"))v.waste=decltype(v.waste)(j["waste"].num());
if(j.has("photo"))v.photo=decltype(v.photo)(j["photo"].num());
if(j.has("ts"))v.ts=decltype(v.ts)(j["ts"].num());
if(j.has("tw"))v.tw=decltype(v.tw)(j["tw"].num());
if(j.has("ms"))v.ms=decltype(v.ms)(j["ms"].num());
if(j.has("mw"))v.mw=decltype(v.mw)(j["mw"].num());
if(j.has("res"))v.res=decltype(v.res)(j["res"].num());
if(j.has("division"))v.division=decltype(v.division)(j["division"].num());
if(j.has("lock"))v.lock=decltype(v.lock)(j["lock"].num());
if(j.has("damage"))v.damage=decltype(v.damage)(j["damage"].num());
if(j.has("starve"))v.starve=decltype(v.starve)(j["starve"].num());
if(j.has("toxic"))v.toxic=decltype(v.toxic)(j["toxic"].num());
if(j.has("activity"))v.activity=decltype(v.activity)(j["activity"].num());
if(j.has("px"))v.px=decltype(v.px)(j["px"].num());
if(j.has("py"))v.py=decltype(v.py)(j["py"].num());
if(j.has("vx"))v.vx=decltype(v.vx)(j["vx"].num());
if(j.has("vy"))v.vy=decltype(v.vy)(j["vy"].num());
if(j.has("exposure"))v.exposure=decltype(v.exposure)(j["exposure"].num());
if(j.has("pressure"))v.pressure=decltype(v.pressure)(j["pressure"].num());
if(j.has("aspect"))v.aspect=decltype(v.aspect)(j["aspect"].num());
if(j.has("development"))v.development=decltype(v.development)(j["development"].num());
if(j.has("light"))v.light=decltype(v.light)(j["light"].num());
if(j.has("remaining"))v.remaining=decltype(v.remaining)(j["remaining"].num());
if(j.has("fragmentReady"))v.fragmentReady=decltype(v.fragmentReady)(j["fragmentReady"].num());
if(j.has("budDomain"))v.budDomain=decltype(v.budDomain)(j["budDomain"].num());
if(j.has("budHop"))v.budHop=decltype(v.budHop)(j["budHop"].num());
if(j.has("budAge"))v.budAge=decltype(v.budAge)(j["budAge"].num());
if(j.has("budQuorum"))v.budQuorum=decltype(v.budQuorum)(j["budQuorum"].num());
if(j.has("budCommit"))v.budCommit=decltype(v.budCommit)(j["budCommit"].num());
if(j.has("budRest"))v.budRest=decltype(v.budRest)(j["budRest"].num());
if(j.has("budX"))v.budX=decltype(v.budX)(j["budX"].num());
if(j.has("budY"))v.budY=decltype(v.budY)(j["budY"].num());
if(j.has("budLink"))v.budLink=j["budLink"].num();if(j.has("budLostTime"))v.budLostTime=j["budLostTime"].num();
if(j.has("budMutationLoci"))v.budMutationLoci=int(j["budMutationLoci"].num());if(j.has("budGeneApplied"))v.budGeneApplied=j["budGeneApplied"].boolean();
if(j.has("budGenome")){if(j["budGenome"].a.size()!=v.budGenome.size())throw std::runtime_error("Invalid bud genetic signal");for(size_t i=0;i<v.budGenome.size();i++)v.budGenome[i]=j["budGenome"].a[i].num();}

if(j.has("alive"))v.alive=decltype(v.alive)(j["alive"].boolean());
if(j.has("maintenance"))v.maintenance=decltype(v.maintenance)(j["maintenance"].boolean());
if(j.has("genome")){if(j["genome"].a.size()!=v.genome.size())throw std::runtime_error("Invalid genome size");for(size_t i=0;i<v.genome.size();i++)v.genome[i]=j["genome"].a[i].num();}
if(j.has("rates")){if(j["rates"].a.size()!=v.rates.size())throw std::runtime_error("Invalid rates size");for(size_t i=0;i<v.rates.size();i++)v.rates[i]=j["rates"].a[i].num();}
if(j.has("observed")){if(j["observed"].a.size()!=v.observed.size())throw std::runtime_error("Invalid observed size");for(size_t i=0;i<v.observed.size();i++)v.observed[i]=j["observed"].a[i].num();}
if(j.has("signals")){if(j["signals"].a.size()!=v.signals.size())throw std::runtime_error("Invalid signal state");for(size_t i=0;i<v.signals.size();i++)v.signals[i]=j["signals"].a[i].num();}
return v;}
inline Json stateJson(const Simulation& s){Json j=Json::object();j["format"]="native-v6-state-1";j["config"]=s.cfg.json();
j["nextBudId"]=double(s.nextBudId);j["budEvents"]=s.budEvents;
j["nextColonyId"]=double(s.nextColonyId);
j["fragmentBirths"]=double(s.fragmentBirths);
j["fragmentMutationLoci"]=double(s.fragmentMutationLoci);
j["steps"]=double(s.steps);
j["nextId"]=double(s.nextId);
j["time"]=double(s.time);
j["prod"]=double(s.prod);
j["cons"]=double(s.cons);
j["cleared"]=double(s.cleared);
j["births"]=double(s.births);
j["deaths"]=double(s.deaths);
j["mutations"]=double(s.mutations);
j["growthSpent"]=double(s.growthSpent);
j["pumpSpent"]=double(s.pumpSpent);
j["regulationSpent"]=double(s.regulationSpent);
j["divisionSpent"]=double(s.divisionSpent);
j["wasteProduced"]=double(s.wasteProduced);
j["recycledS"]=double(s.recycledS);
j["recycledW"]=double(s.recycledW);
j["inputS"]=double(s.inputS);
j["inputW"]=double(s.inputW);
j["clearedS"]=double(s.clearedS);
j["initialS"]=double(s.initialS);
j["initialW"]=double(s.initialW);
j["initialMass"]=double(s.initialMass);
j["grownMass"]=double(s.grownMass);
j["decomposedMass"]=double(s.decomposedMass);
std::ostringstream rngState;rngState<<s.rng;j["rng"]=rngState.str();j["cells"]=Json::array();for(auto&v:s.c)j["cells"].push(cellJson(v));
j["sugarField"]=Json::array();for(auto v:s.sugarField)j["sugarField"].push(double(v));
j["wasteField"]=Json::array();for(auto v:s.wasteField)j["wasteField"].push(double(v));
j["homeo"]=Json::array();for(auto v:s.homeo)j["homeo"].push(double(v));
j["density"]=Json::array();for(auto v:s.density)j["density"].push(double(v));
j["lightField"]=Json::array();for(auto v:s.lightField)j["lightField"].push(double(v));
j["tissue"]=Json::array();for(auto v:s.tissue)j["tissue"].push(double(v));
j["exterior"]=Json::array();for(auto v:s.exterior)j["exterior"].push(double(v));
j["membrane"]=Json::array();for(auto v:s.membrane)j["membrane"].push(double(v));
j["edges"]=Json::array();for(auto&e:s.edges){Json a=Json::array();a.push(e.a);a.push(e.b);a.push(e.sugar);a.push(e.waste);j["edges"].push(a);}
j["mechanical"]=Json::array();for(auto&e:s.mechanical){Json a=Json::array();a.push(e.a);a.push(e.b);a.push(e.sugar);a.push(e.waste);j["mechanical"].push(a);}
j["channels"]=Json::array();for(auto&table:s.channels){Json a=Json::array();for(auto&[key,v]:table){Json row=Json::array();row.push(key.first);row.push(key.second);row.push(v.strength);row.push(v.direction);row.push(v.observed);a.push(row);}j["channels"].push(a);}return j;}
inline std::unique_ptr<Simulation> readState(const Json& j){if(j["format"].str()!="native-v6-state-1")throw std::runtime_error("Unsupported checkpoint format");auto s=std::make_unique<Simulation>(Config::from(j["config"]));
s->steps=decltype(s->steps)(j["steps"].num());
s->nextId=decltype(s->nextId)(j["nextId"].num());
if(j.has("nextBudId"))s->nextBudId=int(j["nextBudId"].num());if(j.has("budEvents"))s->budEvents=j["budEvents"];
if(j.has("nextColonyId"))s->nextColonyId=decltype(s->nextColonyId)(j["nextColonyId"].num());
if(j.has("fragmentBirths"))s->fragmentBirths=decltype(s->fragmentBirths)(j["fragmentBirths"].num());
if(j.has("fragmentMutationLoci"))s->fragmentMutationLoci=decltype(s->fragmentMutationLoci)(j["fragmentMutationLoci"].num());
s->time=decltype(s->time)(j["time"].num());
s->prod=decltype(s->prod)(j["prod"].num());
s->cons=decltype(s->cons)(j["cons"].num());
s->cleared=decltype(s->cleared)(j["cleared"].num());
s->births=decltype(s->births)(j["births"].num());
s->deaths=decltype(s->deaths)(j["deaths"].num());
s->mutations=decltype(s->mutations)(j["mutations"].num());
s->growthSpent=decltype(s->growthSpent)(j["growthSpent"].num());
s->pumpSpent=decltype(s->pumpSpent)(j["pumpSpent"].num());
s->regulationSpent=decltype(s->regulationSpent)(j["regulationSpent"].num());
s->divisionSpent=decltype(s->divisionSpent)(j["divisionSpent"].num());
s->wasteProduced=decltype(s->wasteProduced)(j["wasteProduced"].num());
s->recycledS=decltype(s->recycledS)(j["recycledS"].num());
s->recycledW=decltype(s->recycledW)(j["recycledW"].num());
s->inputS=decltype(s->inputS)(j["inputS"].num());
s->inputW=decltype(s->inputW)(j["inputW"].num());
s->clearedS=decltype(s->clearedS)(j["clearedS"].num());
s->initialS=decltype(s->initialS)(j["initialS"].num());
s->initialW=decltype(s->initialW)(j["initialW"].num());
s->initialMass=decltype(s->initialMass)(j["initialMass"].num());
s->grownMass=decltype(s->grownMass)(j["grownMass"].num());
s->decomposedMass=decltype(s->decomposedMass)(j["decomposedMass"].num());
if(j.has("rng")){std::istringstream input(j["rng"].str());if(!(input>>s->rng))throw std::runtime_error("Invalid RNG state");}s->c.clear();for(auto&v:j["cells"].a)s->c.push_back(readCell(v));
s->sugarField.clear();for(auto&v:j["sugarField"].a)s->sugarField.push_back(decltype(s->sugarField)::value_type(v.num()));
s->wasteField.clear();for(auto&v:j["wasteField"].a)s->wasteField.push_back(decltype(s->wasteField)::value_type(v.num()));
s->homeo.clear();for(auto&v:j["homeo"].a)s->homeo.push_back(decltype(s->homeo)::value_type(v.num()));
s->density.clear();for(auto&v:j["density"].a)s->density.push_back(decltype(s->density)::value_type(v.num()));
s->lightField.clear();for(auto&v:j["lightField"].a)s->lightField.push_back(decltype(s->lightField)::value_type(v.num()));
s->tissue.clear();for(auto&v:j["tissue"].a)s->tissue.push_back(decltype(s->tissue)::value_type(v.num()));
s->exterior.clear();for(auto&v:j["exterior"].a)s->exterior.push_back(decltype(s->exterior)::value_type(v.num()));
s->membrane.clear();for(auto&v:j["membrane"].a)s->membrane.push_back(decltype(s->membrane)::value_type(v.num()));
s->edges.clear();for(auto&v:j["edges"].a){if(v.a.size()!=4)throw std::runtime_error("Invalid edge");s->edges.push_back({int(v.a[0].num()),int(v.a[1].num()),v.a[2].num(),v.a[3].num()});}
s->mechanical.clear();for(auto&v:j["mechanical"].a){if(v.a.size()!=4)throw std::runtime_error("Invalid edge");s->mechanical.push_back({int(v.a[0].num()),int(v.a[1].num()),v.a[2].num(),v.a[3].num()});}
if(j["channels"].a.size()!=2)throw std::runtime_error("Invalid channels");for(int i=0;i<2;i++){s->channels[i].clear();for(auto&v:j["channels"].a[i].a){if(v.a.size()!=5)throw std::runtime_error("Invalid channel");s->channels[i][{int(v.a[0].num()),int(v.a[1].num())}]={v.a[2].num(),v.a[3].num(),v.a[4].num()};}}
size_t size=size_t(s->cfg.grid_size)*s->cfg.grid_size;if(s->cfg.max_cells>0&&s->c.size()>size_t(s->cfg.max_cells))throw std::runtime_error("Invalid cell count");
if(s->sugarField.size()!=size)throw std::runtime_error("Invalid sugarField grid size");
if(s->wasteField.size()!=size)throw std::runtime_error("Invalid wasteField grid size");
if(s->homeo.size()!=size)throw std::runtime_error("Invalid homeo grid size");
if(s->density.size()!=size)throw std::runtime_error("Invalid density grid size");
if(s->lightField.size()!=size)throw std::runtime_error("Invalid lightField grid size");
if(s->tissue.size()!=size)throw std::runtime_error("Invalid tissue grid size");
if(s->exterior.size()!=size)throw std::runtime_error("Invalid exterior grid size");
for(auto*list:{&s->edges,&s->mechanical})for(auto&e:*list)if(e.a<0||e.b<0||e.a>=int(s->c.size())||e.b>=int(s->c.size()))throw std::runtime_error("Invalid contact index");return s;}
}
