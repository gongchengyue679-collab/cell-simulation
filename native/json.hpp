#pragma once
#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <cstdlib>
namespace cs {
// Small strict JSON value: no runtime or external DLL dependencies.
struct Json {
 enum Type {Null, Number, Boolean, String, Array, Object} type=Null;
 double n=0; std::string s; std::vector<Json> a; std::map<std::string,Json> o;
 Json()=default; Json(double v):type(Number),n(v){} Json(int v):Json(double(v)){} Json(bool v):type(Boolean),n(v){}
 Json(const char* v):type(String),s(v){} Json(std::string v):type(String),s(std::move(v)){}
 static Json array(){Json v;v.type=Array;return v;} static Json object(){Json v;v.type=Object;return v;}
 bool has(const std::string& k) const{return o.contains(k);} bool null() const{return type==Null;}
 Json& operator[](const std::string& k){if(type==Null)type=Object;if(type!=Object)throw std::runtime_error("Expected JSON object");return o[k];}
 const Json& operator[](const std::string& k) const{static const Json nil;auto i=o.find(k);return i==o.end()?nil:i->second;}
 double num(double fallback=0)const{return type==Number||type==Boolean?n:fallback;}
 std::string str(std::string fallback="")const{return type==String?s:fallback;}
 bool boolean(bool fallback=false)const{return type==Boolean?n!=0:fallback;}
 void push(Json v){if(type==Null)type=Array;a.push_back(std::move(v));}
 static std::string quote(const std::string& x){std::ostringstream os;os<<'"';for(unsigned char c:x){switch(c){case '"':os<<"\\\"";break;case '\\':os<<"\\\\";break;case '\n':os<<"\\n";break;case '\r':os<<"\\r";break;case '\t':os<<"\\t";break;default:if(c<32)os<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<int(c);else os<<c;}}os<<'"';return os.str();}
 std::string dump()const{std::ostringstream os;os<<std::setprecision(17);switch(type){case Null:return "null";case Number:if(!std::isfinite(n))throw std::runtime_error("Nonfinite JSON number");os<<n;break;case Boolean:os<<(n?"true":"false");break;case String:return quote(s);case Array:os<<'[';for(size_t i=0;i<a.size();++i){if(i)os<<',';os<<a[i].dump();}os<<']';break;case Object:os<<'{';{bool first=true;for(auto&[k,v]:o){if(!first)os<<',';first=false;os<<quote(k)<<':'<<v.dump();}}os<<'}';}return os.str();}
 static Json parse(const std::string& text){size_t p=0;auto ws=[&](){while(p<text.size()&&isspace((unsigned char)text[p]))++p;};auto fail=[](){throw std::runtime_error("Invalid JSON");};
  auto readString=[&](){std::string out;if(p>=text.size()||text[p++]!='"')fail();while(p<text.size()){unsigned char c=text[p++];if(c=='"')return out;if(c<32)fail();if(c!='\\'){out+=c;continue;}if(p>=text.size())fail();c=text[p++];switch(c){case '"':case '\\':case '/':out+=c;break;case 'b':out+='\b';break;case 'f':out+='\f';break;case 'n':out+='\n';break;case 'r':out+='\r';break;case 't':out+='\t';break;case 'u':{unsigned code=0;for(int j=0;j<4;++j){if(p>=text.size())fail();char h=text[p++];code*=16;if(h>='0'&&h<='9')code+=h-'0';else if(h>='a'&&h<='f')code+=h-'a'+10;else if(h>='A'&&h<='F')code+=h-'A'+10;else fail();}if(code<128)out+=char(code);else if(code<2048){out+=char(192|(code>>6));out+=char(128|(code&63));}else{out+=char(224|(code>>12));out+=char(128|((code>>6)&63));out+=char(128|(code&63));}break;}default:fail();}}fail();return out;};
  auto read=[&](auto&& self,int depth)->Json{if(depth>128)fail();ws();if(p>=text.size())fail();char c=text[p];if(c=='"')return readString();if(c=='{'||c=='['){++p;Json out=c=='{'?object():array();ws();char end=c=='{'?'}':']';if(p<text.size()&&text[p]==end){++p;return out;}while(true){ws();if(c=='{'){auto key=readString();ws();if(p>=text.size()||text[p++]!=':')fail();if(out.has(key))fail();out[key]=self(self,depth+1);}else out.push(self(self,depth+1));ws();if(p>=text.size())fail();char d=text[p++];if(d==end)return out;if(d!=',')fail();}}
   for(auto [literal,v]:std::vector<std::pair<std::string,Json>>{{"true",true},{"false",false},{"null",Json()}})if(text.compare(p,literal.size(),literal)==0){p+=literal.size();return v;}
   size_t start=p;if(c=='-')++p;if(p>=text.size())fail();if(text[p]=='0')++p;else{if(text[p]<'1'||text[p]>'9')fail();while(p<text.size()&&isdigit((unsigned char)text[p]))++p;}if(p<text.size()&&text[p]=='.'){++p;size_t b=p;while(p<text.size()&&isdigit((unsigned char)text[p]))++p;if(p==b)fail();}if(p<text.size()&&(text[p]=='e'||text[p]=='E')){++p;if(p<text.size()&&(text[p]=='+'||text[p]=='-'))++p;size_t b=p;while(p<text.size()&&isdigit((unsigned char)text[p]))++p;if(p==b)fail();}double d=std::stod(text.substr(start,p-start));if(!std::isfinite(d))fail();return d;};
  Json out=read(read,0);ws();if(p!=text.size())fail();return out;
 }
 static Json load(const std::filesystem::path& path){std::ifstream f(path,std::ios::binary);if(!f)throw std::runtime_error("Cannot open JSON file: "+path.string());std::string t((std::istreambuf_iterator<char>(f)),{});if(t.starts_with("\xef\xbb\xbf"))t.erase(0,3);return parse(t);}
 void save(const std::filesystem::path& path)const{if(!path.parent_path().empty())std::filesystem::create_directories(path.parent_path());std::ofstream f(path,std::ios::binary);f<<dump()<<'\n';if(!f)throw std::runtime_error("Cannot write JSON file");}
};
}
