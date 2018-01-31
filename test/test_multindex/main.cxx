/* Boost.MultiIndex basic example.
 *
 * Copyright 2003-2013 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/multi_index for library home page.
 */

#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

#include <time.h>
#include <string.h>
#include <stdio.h>

using boost::multi_index_container;
using namespace boost::multi_index;

/*
time_t first conection
time_t last conection
random seed para dar 'salt' na session...
ip
session-id
path
*/
extern "C"
{
//keccak
void FIPS202_SHAKE128(const unsigned char *input, unsigned int inputByteLen, unsigned char *output, int outputByteLen);
}
char Hex2Char(char c)
{
   switch(c)
   {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
         return c+'0';
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
         return c+55;//'7'
   }
}

template<typename Tag,typename MultiIndexContainer>
void print_out_by(const MultiIndexContainer& s)
{
  // obtain a reference to the index tagged by Tag 
  const typename boost::multi_index::index<MultiIndexContainer,Tag>::type& i= get<Tag>(s);
  typedef typename MultiIndexContainer::value_type value_type;
  // dump the elements of the index to cout 
  std::copy(i.begin(),i.end(),std::ostream_iterator<value_type>(std::cout));
}

struct Field
{
   std::string key;
   std::string value;
   Field(std::string &k,std::string &v):key(k),value(v){}
   Field(const char *k,const char *v):key(k),value(v){}
   friend std::ostream& operator<<(std::ostream& os,const Field& f)
   {
      os<< "\t " << f.key << " -> "
         << f.value << std::endl;
      return os;
   }

};
struct key{};
struct value{};

typedef multi_index_container<
  Field,
  indexed_by<
    ordered_unique<
      tag<key>, BOOST_MULTI_INDEX_MEMBER(Field,std::string,key)
    >,
    ordered_non_unique<
      tag<value>, BOOST_MULTI_INDEX_MEMBER(Field,std::string,value)
    >
  > 
> Field_set;

struct Session
{
   
   time_t first;
   time_t last;
   unsigned long int salt;
   std::string ip;
   std::string session_id;
   std::string path;

   Field_set fields;

   Session(){}
   Session(time_t _first, time_t _last,unsigned long int _salt,const char *_ip,const char*_path):
      first(_first),last(_last),salt(_salt),ip(_ip),path(_path)
   {
      int i,j;
      char input[128];
      char output[128];
      memset(input,0,128);
      memset(output,0,128);
      sprintf(input,"%lu,%lu,%lu,%s,%s",_first,_last,_salt,_ip,_path);
      printf("%s\n",input);
      FIPS202_SHAKE128((const unsigned char*)input,strlen((const char*)input),(unsigned char*)output,128);
      for(i=0,j=0;i<32;i++){
         input[j++]=Hex2Char((output[i]&0xf0)>>4);
         input[j++]=Hex2Char(output[i]&0xf);
      }
      session_id=input;
   }
   friend std::ostream& operator<<(std::ostream& os,const Session& s)
   {
      os<< s.first << " "
         << s.last << " "
         << s.salt << " "
         << s.ip << " "
         << s.session_id << " "
         << s.path << std::endl;
      return os;
   }
   int AddField(const char *k, const char *v)
   {
      fields.insert(Field(k,v));
   }
   void PrintSession()const 
   {
      std::cout<< "Session:" <<std::endl;
      std::cout<< "\t first:"<< first <<std::endl;
      std::cout<< "\t last:"<< last <<std::endl;
      std::cout<< "\t salt:"<< salt <<std::endl;
      std::cout<< "\t ip:"<< ip <<std::endl;
      std::cout<< "\t session_id:"<< session_id <<std::endl;
      std::cout<< "\t path:"<< path <<std::endl;
      
      std::cout<<"Fields"<<std::endl;
      print_out_by<key>(fields);
      std::cout<<std::endl;

   }
};
struct last{};
struct ip{};
struct session_id{};
struct path{};

typedef multi_index_container<
  Session,
  indexed_by<
    ordered_unique<
      tag<session_id>, BOOST_MULTI_INDEX_MEMBER(Session,std::string,session_id)>,
    ordered_non_unique<
      tag<last>,  BOOST_MULTI_INDEX_MEMBER(Session,time_t,last)>,
    ordered_non_unique<
      tag<ip>,BOOST_MULTI_INDEX_MEMBER(Session,std::string,ip)>,
    ordered_non_unique<
      tag<path>, BOOST_MULTI_INDEX_MEMBER(Session,std::string,path)>
   >
> Session_set;

int find_session(Session_set &ss,const char*_ip,Session &s)
{
   typedef Session_set::index<ip>::type session_set_by_ip;
   session_set_by_ip& name_index=ss.get<ip>();
   session_set_by_ip::iterator it=name_index.find(_ip);
   if(it!=name_index.end())
   {
      s=*it;
      return 1;
   }else
      return 0;
}
int main()
{
  time_t t;
  Session session(
   time(NULL),
   time(NULL)+1000,
   random(),
   "127.0.0.1",
   "/tefmock/comanda"
   );
   Session_set ss;

   srand(456789);
   char _ip[17];
   int i=1;

   sprintf(_ip,"127.0.0.%d",i++);
   t=time(NULL)+random();
   ss.insert(Session(t,t+random(),random(),_ip,"/tefmock/service"));

   sprintf(_ip,"127.0.0.%d",i++);
   t=time(NULL)+random();
   ss.insert(Session(t,t+random(),random(),_ip,"/tefmock/comanda"));

   sprintf(_ip,"127.0.0.%d",i++);
   t=time(NULL)+random();
   ss.insert(Session(t,t+random(),random(),_ip,"/tefmock/estudos"));

   // next insertion will fail, as there is an employee with
   // the same ID

   sprintf(_ip,"127.0.0.%d",i++);
   t=time(NULL)+random();
   ss.insert(Session(t,t+random(),random(),_ip,"/tefmock/estudos"));

   sprintf(_ip,"127.0.0.%d",i++);
   t=time(NULL)+random();
   ss.insert(Session(t,t+random(),random(),_ip,"/tefmock/estudos"));

   sprintf(_ip,"127.0.0.%d",i++);
   t=time(NULL)+random();
   ss.insert(Session(t,t+random(),random(),_ip,"/tefmock/estudos"));

   std::cout<<"by last"<<std::endl;
   print_out_by<last>(ss);
   std::cout<<std::endl;

   std::cout<<"by ip"<<std::endl;
   print_out_by<ip>(ss);
   std::cout<<std::endl;

   std::cout<<"by session_id"<<std::endl;
   print_out_by<session_id>(ss);
   std::cout<<std::endl;

   std::cout<<"by path"<<std::endl;
   print_out_by<path>(ss);
   std::cout<<std::endl;

   Session s;
   int ret = find_session(ss,"127.0.0.6",s);
   if(ret){
      s.fields.insert(Field("key","value"));
      s.fields.insert(Field("index","aaa"));
      s.fields.insert(Field("last_page","/empreendedor/varios/reference.html"));
      s.PrintSession();
   }else
      std::cout << "failed to find session" << std::endl;
   //session.PrintSession();
   return 0;
}
