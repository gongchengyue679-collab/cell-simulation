#pragma once
#include "simulation.hpp"
#include <queue>
#include <functional>
namespace cs {
struct FlowEdge {int to,reverse;double cap;};
struct FlowResult {double total=0;std::vector<double>taken,received;};
inline FlowResult assay(int n,const std::vector<std::tuple<int,int,double>>&edges,const std::vector<double>&supply,const std::vector<double>&demand){
 std::vector<std::vector<FlowEdge>>graph(n+2);int source=n,sink=n+1;
 auto add=[&](int a,int b,double cap){int index=int(graph[a].size());graph[a].push_back({b,int(graph[b].size()),cap});graph[b].push_back({a,index,0});return index;};
 for(auto[a,b,cap]:edges)add(a,b,cap);std::vector<int>src(n),dst(n);for(int i=0;i<n;i++){src[i]=add(source,i,supply[i]);dst[i]=add(i,sink,demand[i]);}
 FlowResult result;result.taken.resize(n);result.received.resize(n);
 for(;;){std::vector<int>level(n+2,-1),cursor(n+2);std::queue<int>queue;queue.push(source);level[source]=0;while(!queue.empty()){int a=queue.front();queue.pop();for(auto&e:graph[a])if(e.cap>1e-12&&level[e.to]<0){level[e.to]=level[a]+1;queue.push(e.to);}}if(level[sink]<0)break;
  std::function<double(int,double)>send=[&](int a,double limit){if(a==sink)return limit;for(int&i=cursor[a];i<int(graph[a].size());i++){auto&e=graph[a][i];if(e.cap>1e-12&&level[e.to]==level[a]+1){double sent=send(e.to,std::min(limit,e.cap));if(sent>1e-12){e.cap-=sent;graph[e.to][e.reverse].cap+=sent;return sent;}}}return 0.;};
  for(;;){double sent=send(source,1e100);if(sent<=1e-12)break;result.total+=sent;}
 }
 for(int i=0;i<n;i++){result.taken[i]=supply[i]-graph[source][src[i]].cap;result.received[i]=demand[i]-graph[i][dst[i]].cap;}return result;
}
inline Json tissueMetrics(const Simulation&s){
 std::vector<int>parent(s.c.size());std::iota(parent.begin(),parent.end(),0);auto root=[&](int i){while(parent[i]!=i){parent[i]=parent[parent[i]];i=parent[i];}return i;};
 for(auto&e:s.edges)if(s.c[e.a].alive&&s.c[e.b].alive)parent[root(e.a)]=root(e.b);
 std::map<int,std::vector<int>>groups;double mass=0;for(int i=0;i<int(s.c.size());i++)if(s.c[i].alive){groups[root(i)].push_back(i);mass+=s.c[i].mass;}
 std::vector<int>component;for(auto&[key,group]:groups)if(group.size()>component.size())component=group;int n=int(component.size());std::map<int,int>local;int healthy=0;for(int i=0;i<n;i++){auto&v=s.c[component[i]];local[v.id]=i;healthy+=v.status==0&&v.damage<.5;}
 Json j=Json::object();j["time"]=s.time;j["connected_living"]=n;j["healthy_fraction"]=n?double(healthy)/n:0;j["waste_per_biomass"]=n?s.total(true)/std::max(mass,1e-12):0;
 std::vector<double>sugarSupply(n),sugarNeed(n),wasteSupply(n),wasteSink(n);int dependents=0,producers=0;
 for(int i=0;i<n;i++){auto&v=s.c[component[i]];auto&r=v.observed;double consumption=r[1]+r[2]+r[6]+r[7];sugarSupply[i]=std::max(r[0]-consumption,0.);sugarNeed[i]=std::max(consumption-r[0],0.);if(sugarNeed[i]<1e-5)sugarNeed[i]=0;dependents+=sugarNeed[i]>0;wasteSupply[i]=v.exposure<.25?r[3]:0;wasteSink[i]=v.exposure>=.25?std::max(r[5],0.):0;producers+=wasteSupply[i]>1e-8;}
 j["sugar_dependents"]=dependents;j["waste_producers"]=producers;
 for(int cargo=0;cargo<2;cargo++){std::string name=cargo?"waste":"sugar";auto&supply=cargo?wasteSupply:sugarSupply;auto&demand=cargo?wasteSink:sugarNeed;std::vector<std::tuple<int,int,double>>edges;
  for(auto&contact:s.edges){Key key{s.c[contact.a].id,s.c[contact.b].id};auto channel=s.channels[cargo].find(key);if(channel==s.channels[cargo].end())continue;auto&state=channel->second;if(local.contains(key.first)&&local.contains(key.second)&&state.strength>=.35&&std::abs(state.observed)>=1e-8){int a=local.at(key.first),b=local.at(key.second);if(state.observed<0)std::swap(a,b);edges.emplace_back(a,b,std::abs(state.observed));}}
  auto flow=assay(n,edges,supply,demand);auto&needed=cargo?supply:demand;auto&serviced=cargo?flow.taken:flow.received;int eligible=0,covered=0;double required=0;for(int i=0;i<n;i++){required+=needed[i];if(needed[i]>1e-8){eligible++;covered+=serviced[i]>=.25*needed[i];}}
  j[name+"_coverage"]=eligible?double(covered)/eligible:0;j[name+"_delivery"]=flow.total/std::max(required,1e-12);j[name+"_delivered_rate"]=flow.total;j[name+"_edges"]=int(edges.size());
  std::vector<std::vector<int>>adj(n);for(auto[a,b,cap]:edges)if(cargo)adj[b].push_back(a);else adj[a].push_back(b);std::vector<int>distance(n,-1);std::queue<int>q;for(int i=0;i<n;i++)if(cargo?(demand[i]>1e-8&&flow.received[i]>1e-8):supply[i]>1e-8){distance[i]=0;q.push(i);}while(!q.empty()){int a=q.front();q.pop();for(int b:adj[a])if(distance[b]<0){distance[b]=distance[a]+1;q.push(b);}}int longest=0;for(int i=0;i<n;i++)if(cargo?(supply[i]>1e-8&&flow.taken[i]>1e-8):(demand[i]>1e-8&&flow.received[i]>1e-8))longest=std::max(longest,distance[i]);j[name+"_path_length"]=longest;
 }return j;
}
inline Json defaultTarget(){return Json::parse(R"({"living":300,"stable_time":100,"healthy_fraction":0.8,"dependent_fraction":0.1,"coverage":0.7,"delivery":0.5,"min_edges":10,"min_path":3,"window_pass_fraction":0.9,"max_waste_density":0.5,"max_waste_slope":0.001,"validation_seeds":10,"validation_required":8,"recovery_time":100,"recovery_fraction":0.7})");}
inline bool samplePasses(const Json&m,const Json&t){double n=m["connected_living"].num();if(n<t["living"].num()||m["healthy_fraction"].num()<t["healthy_fraction"].num()||m["sugar_dependents"].num()<std::max(3.,std::floor(n*t["dependent_fraction"].num()))||m["waste_producers"].num()<std::max(3.,std::floor(n*t["dependent_fraction"].num())))return false;for(auto name:{std::string("sugar"),std::string("waste")})if(m[name+"_coverage"].num()<t["coverage"].num()||m[name+"_delivery"].num()<t["delivery"].num()||m[name+"_edges"].num()<t["min_edges"].num()||m[name+"_path_length"].num()<t["min_path"].num())return false;return true;}
inline bool wasteStable(const std::vector<Json>&rows,const Json&t){if(rows.empty())return false;size_t width=std::max(size_t(1),rows.size()/4);double first=0,last=0,peak=0;for(size_t i=0;i<rows.size();i++){double x=rows[i]["waste_per_biomass"].num();peak=std::max(peak,x);if(i<width)first+=x;if(i>=rows.size()-width)last+=x;}double span=std::max(1.,rows.back()["time"].num()-rows.front()["time"].num());return peak<=t["max_waste_density"].num()&&(last-first)/width/(.75*span)<=t["max_waste_slope"].num();}
}
