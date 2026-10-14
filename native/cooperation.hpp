// Included inside Simulation. Control rules use only self and current contacts.
// The final component scan records lineage; it never changes forces or genomes.
void propagateWiredSignals(){
 std::vector<std::array<double,2>> received(c.size());std::vector<double> weights(c.size());
 for(auto&e:edges){
  double strength=0;for(auto&table:channels){auto it=table.find({c[e.a].id,c[e.b].id});if(it!=table.end())strength=std::max(strength,it->second.strength);}
  double wire=.25+.75*strength;weights[e.a]+=wire;weights[e.b]+=wire;
  for(int k=0;k<2;k++){received[e.a][k]+=wire*c[e.b].signals[k];received[e.b][k]+=wire*c[e.a].signals[k];}
 }
 double response=-std::expm1(-cfg.communication_response*cfg.dt);
 for(size_t i=0;i<c.size();i++)if(c[i].alive){
  auto&v=c[i];double own[]={clip(1-v.sugar/volume(v)/cfg.sugar_low),clip(v.waste/volume(v)/std::max(v.res*v.genome[7],.05))};
  double requested=0;std::array<double,2> target;
  for(int k=0;k<2;k++){double input=weights[i]?received[i][k]/weights[i]:0.;target[k]=clip((.45*own[k]+.85*input)*std::exp(-cfg.signal_decay*cfg.dt));requested+=own[k]+input+target[k];}
  double cost=cfg.signal_cost*requested*cfg.dt,paid=std::min(v.sugar,cost),available=cost>0?paid/cost:1.;
  for(int k=0;k<2;k++)v.signals[k]+=response*(target[k]*available-v.signals[k]);
  v.sugar-=paid;v.waste+=paid*cfg.waste_yield;regulationSpent+=paid;wasteProduced+=paid*cfg.waste_yield;v.rates[7]+=paid/cfg.dt;v.rates[3]+=paid*cfg.waste_yield/cfg.dt;
 }
}

void propagateCooperativeBuds(){
 if(!cfg.fragmentation_enabled)return;
 struct Local{int domain,hop,changed;double age,quorum,commit,x,y,link;std::array<double,59>genome;};
 std::vector<Local> old;old.reserve(c.size());for(auto&v:c)old.push_back({v.budDomain,v.budHop,v.budMutationLoci,v.budAge,v.budQuorum,v.budCommit,v.budX,v.budY,v.budLink,v.budGenome});
 std::vector<std::vector<int>> neighbors(c.size());for(auto&e:edges){neighbors[e.a].push_back(e.b);neighbors[e.b].push_back(e.a);}
 std::uniform_real_distribution<double> random(0,1);std::normal_distribution<double> mutation(0,cfg.fragment_mutation_sigma);double response=-std::expm1(-1.3*cfg.dt);
 for(size_t i=0;i<c.size();i++)if(c[i].alive){
  auto&v=c[i];v.budRest=std::max(0.,v.budRest-cfg.dt);
  if(v.budDomain&&v.budAge>=cfg.bud_lifetime){v.budDomain=0;v.budHop=0;v.budAge=v.budQuorum=v.budCommit=v.budLink=v.budLostTime=0;v.budRest=cfg.bud_refractory_time;}
  if(!v.budDomain&&v.budRest<=0){
   int relay=-1;bool inhibited=false;
   for(int j:neighbors[i])if(old[j].domain){inhibited=true;if(old[j].hop<cfg.bud_signal_hops&&old[j].age<cfg.bud_maturation_time&&old[j].commit<.15&&
      (relay<0||old[j].hop<old[relay].hop||(old[j].hop==old[relay].hop&&old[j].domain<old[relay].domain)))relay=j;}
   if(relay>=0&&v.activity>.35){auto&q=old[relay];v.budDomain=q.domain;v.budHop=q.hop+1;v.budAge=q.age;v.budQuorum=1;v.budX=q.x;v.budY=q.y;v.budGenome=q.genome;v.budMutationLoci=q.changed;v.budGeneApplied=false;}
   else if(!inhibited&&neighbors[i].size()>=3&&v.exposure>.12&&v.fragmentReady>=cfg.fragment_signal_threshold&&
           random(rng)<-std::expm1(-cfg.bud_initiation_rate*cfg.dt)){
    v.budDomain=nextBudId++;v.budHop=0;v.budAge=0;v.budQuorum=1;v.budX=v.px;v.budY=v.py;
    v.budGenome=v.genome;v.budMutationLoci=0;v.budGeneApplied=false;
    for(size_t k=0;k<v.budGenome.size();k++)if(random(rng)<cfg.fragment_mutation_probability){double next=clip(v.budGenome[k]+mutation(rng),k<10?.5:-1.,k<10?1.8:1.);v.budMutationLoci+=next!=v.budGenome[k];v.budGenome[k]=next;}
   }
  }
  if(!v.budDomain)continue;
  double sum=0,permit=0,link=0;for(int j:neighbors[i])if(old[j].domain==v.budDomain){sum+=old[j].quorum;permit=std::max(permit,old[j].commit*.995);link=std::max(link,.9*old[j].link);}else link=1;
  v.budLink+=(-std::expm1(-2*cfg.dt))*(link-v.budLink);
  v.budLostTime=v.budLink<.15?v.budLostTime+cfg.dt:0;
  v.budAge+=cfg.dt;
  // A leaky contact relay supplies a local quorum estimate; no colony count enters it.
  double target=1+.95*sum/std::max(6.,double(neighbors[i].size()));
  v.budQuorum+=response*(target-v.budQuorum);
  double mature=clip((v.budAge-cfg.bud_maturation_time)/8.);
  permit=std::max(permit,mature*clip((v.budQuorum-cfg.bud_quorum_threshold)/1.5)*clip(v.budLink/.3));
  double cost=cfg.signal_cost*(1+v.budQuorum+permit)*cfg.dt,paid=std::min(v.sugar,cost),available=cost>0?paid/cost:1.;
  v.budCommit=clip(v.budCommit+response*(permit*available-v.budCommit));
  // Genetic activation is a cell's own response to the relayed commitment,
  // independent of disconnected-component detection and lineage labels.
  if(v.budCommit>.45&&!v.budGeneApplied){v.genome=v.budGenome;v.budGeneApplied=true;if(v.budMutationLoci)mutations++;}
  v.sugar-=paid;v.waste+=paid*cfg.waste_yield;regulationSpent+=paid;wasteProduced+=paid*cfg.waste_yield;v.rates[7]+=paid/cfg.dt;v.rates[3]+=paid*cfg.waste_yield/cfg.dt;
  // A whole small tissue has no parent interface. Its cells can observe this only
  // through the disappearing contact relay, then locally abandon the attempt.
  if(v.budAge>cfg.bud_maturation_time&&v.budLostTime>8&&v.budCommit<.15){v.budDomain=0;v.budAge=v.budQuorum=v.budCommit=v.budLink=v.budLostTime=0;v.budRest=15;}
 }
}

double cooperativeBoundary(const Cell&a,const Cell&b)const{
 if(!cfg.fragmentation_enabled||a.budDomain==b.budDomain)return 0.;
 return clip((std::max(a.budCommit,b.budCommit)-.45)/.55);
}
double cooperativeCohesion(const Cell&a,const Cell&b)const{
 return a.budDomain==b.budDomain?2.5:1.;
}

void recordCooperativeBirths(){
 if(!cfg.fragmentation_enabled)return;
 std::map<Key,std::pair<double,double>>flows;for(auto&e:edges)flows[{c[e.a].id,c[e.b].id}]={e.sugar,e.waste};
 updateContacts();for(auto&e:edges){auto it=flows.find({c[e.a].id,c[e.b].id});if(it!=flows.end()){e.sugar=it->second.first;e.waste=it->second.second;}}
 std::vector<std::vector<int>>adj(c.size());for(auto&e:edges){adj[e.a].push_back(e.b);adj[e.b].push_back(e.a);}
 std::map<int,int>remaining;for(auto&v:c)if(v.alive)remaining[v.colonyId]++;
 std::vector<uint8_t>seen(c.size());
 for(size_t i=0;i<c.size();i++)if(c[i].alive&&!seen[i]){
  std::vector<int>component{int(i)};seen[i]=1;for(size_t k=0;k<component.size();k++)for(int other:adj[component[k]])if(!seen[other]){seen[other]=1;component.push_back(other);}
  if(int(component.size())<cfg.fragment_min_cells)continue;
  std::map<int,int>domains,parents;double commit=0;for(int k:component){domains[c[k].budDomain]++;parents[c[k].colonyId]++;commit+=c[k].budCommit;}
  int domain=0,count=0,parent=-1,parentCount=0;for(auto[id,n]:domains)if(id>0&&n>count){domain=id;count=n;}
  for(auto[id,n]:parents)if(n>parentCount){parent=id;parentCount=n;}
  if(!domain||count<.8*component.size()||commit/component.size()<.3||parentCount!=int(component.size())||remaining[parent]-int(component.size())<cfg.fragment_min_cells)continue;
  bool recorded=false;for(auto&event:budEvents.a)if(int(event["domain"].num())==domain)recorded=true;if(recorded)continue;
  int generation=0,changed=0;double mass=0;
  for(int k:component){generation=std::max(generation,c[k].colonyGeneration);mass+=c[k].mass;if(c[k].budDomain==domain&&c[k].budGeneApplied)changed=std::max(changed,c[k].budMutationLoci);}
  int id=nextColonyId++;for(int k:component){auto&v=c[k];v.parentColony=parent;v.colonyId=id;v.colonyGeneration=generation+1;}
  remaining[parent]-=int(component.size());remaining[id]=int(component.size());fragmentBirths++;fragmentMutationLoci+=changed;
  Json event=Json::object();event["domain"]=domain;event["parent"]=parent;event["daughter"]=id;event["generation"]=generation+1;event["time"]=time;event["cells"]=int(component.size());event["mass"]=mass;event["mutated_loci"]=changed;budEvents.push(event);
 }
}
