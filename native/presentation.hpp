#pragma once
#include "simulation.hpp"
#include <gdiplus.h>
namespace cs {
struct ViewInfo {const wchar_t*description;const wchar_t*scale;double maximum;};
inline constexpr ViewInfo viewInfo[]={
 {L"观察细胞健康状态；颜色表示状态，灰色为死亡细胞。",L"状态分类",1},
 {L"显示细胞收到的光照和胞外光场；颜色越暖，光越强。",L"光强（无量纲）",2},
 {L"显示细胞内糖浓度；橙色轮廓表示低于维持储备。",L"胞内糖浓度（无量纲）",4},
 {L"显示细胞内废物浓度；橙色轮廓表示超过该细胞耐受。",L"胞内废物浓度（无量纲）",.25},
 {L"青色箭头表示糖实际流向；线越粗，输送量越大。",L"糖流量 / 模拟时间",.05},
 {L"橙色箭头表示废物实际流向；线越粗，输送量越大。",L"废物流量 / 模拟时间",.01},
 {L"显示已形成的细胞质量；大细胞不一定能立即分裂。",L"细胞质量（初始值 1）",2},
 {L"显示累计损伤；蓝色损伤小，红色接近死亡阈值。",L"损伤 / 死亡阈值",1},
 {L"显示分裂能力；数值高仍需满足质量、健康与空间条件。",L"分裂能力",1},
 {L"细线连接附近的力学邻居；细胞颜色表示健康状态。",L"邻居关系",1},
 {L"颜色表示当前最强的功能投入；不是固定细胞类型。",L"主要功能",1},
 {L"显示分化锁定程度；数值越高，功能调整越缓慢。",L"分化锁定",1},
 {L"显示局部发育状态；它由周围接触和挤压共同决定。",L"发育状态",1},
 {L"白色箭头表示细胞极性方向；细胞颜色表示健康状态。",L"方向箭头",1},
 {L"显示邻居重叠造成的局部挤压；红色表示更强挤压。",L"归一化重叠压力",1},
 {L"青色为糖通道，橙色为废物通道；线越粗通道越强。",L"通道强度（不是即时流量）",1},
 {L"显示相对祖先的基因平均偏离；偏离大不等于更优秀。",L"基因平均绝对偏离",.3},
 {L"显示接触网络传来的出芽协作承诺；蓝色准备，红色协同脱离。",L"出芽协作承诺",1},
 {L"颜色对应团块谱系编号；它用于记录，不决定运动。",L"团块谱系",1}
};
inline Gdiplus::Color palette(double t){
 using Gdiplus::Color;t=clip(t);const int stops[5][3]={{35,64,190},{27,180,224},{60,214,138},{246,213,65},{244,62,53}};
 double z=t*4;int a=std::min(3,int(z));double f=z-a;return Color(255,BYTE(stops[a][0]*(1-f)+stops[a+1][0]*f),BYTE(stops[a][1]*(1-f)+stops[a+1][1]*f),BYTE(stops[a][2]*(1-f)+stops[a+1][2]*f));
}
inline Gdiplus::Color healthColor(int status){using Gdiplus::Color;switch(status){case 0:return Color(83,217,146);case 1:return Color(250,211,65);case 2:return Color(208,105,246);case 3:return Color(247,76,66);default:return Color(125,139,156);}}
inline const wchar_t*healthName(int status){const wchar_t*names[]={L"正常",L"缺糖",L"中毒",L"无法维持",L"死亡"};return names[std::clamp(status,0,4)];}
inline int dominantTrait(const Cell&v){double values[]={v.photo,v.ts,v.tw,v.mw,v.res,v.division};return int(std::max_element(values,values+6)-values);}
inline Gdiplus::Color traitColor(int trait){using Gdiplus::Color;Color colors[]={Color(106,224,109),Color(54,201,242),Color(174,122,250),Color(249,150,64),Color(244,216,72),Color(244,113,178)};return colors[std::clamp(trait,0,5)];}
inline double viewValue(int view,const Cell&v,const Snapshot&s){double volume=std::max(1e-8,v.mass*s.cellVolume);switch(view){case 1:return v.light;case 2:return v.sugar/volume;case 3:return v.waste/volume;case 6:return v.mass;case 7:return v.damage/s.deathThreshold;case 8:return v.division;case 11:return v.lock;case 12:return v.development;case 14:return v.pressure;case 17:return v.budCommit;case 16:{double sum=0;for(int i=0;i<59;i++)sum+=std::abs(v.genome[i]-(i<10?1:0));return sum/59;}default:return 0;}}
inline bool categoricalView(int view){return view==0||view==4||view==5||view==9||view==10||view==13||view==15||view==18;}
}
