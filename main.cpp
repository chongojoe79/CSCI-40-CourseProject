// CSCI-40 Final Project
// Author: [Romero Joseph]
001 #include <bits/stdc++.h>
002 using namespace std;
003 
004 struct Bed { int id; string name; int prio; int intervalMin; int durationSec; long long nextWater; };
005 struct NutrientDose { int bedId; string formula; double ml; long long when; };
006 struct TankReading { double tempC, pH, nh3, no2, no3; long long when; };
007 struct FishFeeding { int tankId; string type; double grams; long long when, nextWhen; };
008 struct WellWater { double hardness, ec; long long when; };
009 
010 string FB="beds.txt", FD="doses.txt", FT="tank.txt", FF="feed.txt", FW="well.txt";
011 long long NOW=0;
012 
013 void clearCin(){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
014 int readInt(string p,int lo=INT_MIN,int hi=INT_MAX){ for(;;){ cout<<p; int v; if(cin>>v){ if(v<lo||v>hi){cout<<"Out of range.\n";continue;} clearCin(); return v; } cout<<"Invalid.\n"; clearCin(); } }
015 double readDouble(string p,double lo=-1e300,double hi=1e300){ for(;;){ cout<<p; double v; if(cin>>v){ if(v<lo||v>hi){cout<<"Out of range.\n";continue;} clearCin(); return v; } cout<<"Invalid.\n"; clearCin(); } }
016 string readLine(string p){ cout<<p; string s; getline(cin,s); return s; }
017 
018 int findBed(const vector<Bed>& v,int id){ for(size_t i=0;i<v.size();++i) if(v[i].id==id) return (int)i; return -1; }
019 
020 void warnTank(const TankReading& r){
021   bool any=false;
022   if(r.pH<6.5||r.pH>7.5){ cout<<"  Warning: pH out of 6.5–7.5.\n"; any=true; }
023   if(r.nh3>=0.25){ cout<<"  Warning: Ammonia >= 0.25 mg/L.\n"; any=true; }
024   if(r.no3>40){ cout<<"  Note: Nitrate > 40 mg/L.\n"; any=true; }
025   if(!any) cout<<"  Status: nominal.\n";
026 }
027 
028 bool saveAll(const vector<Bed>& beds,const vector<NutrientDose>& doses,const vector<TankReading>& tank,const vector<FishFeeding>& feed,const vector<WellWater>& wells){
029   ofstream a(FB),b(FD),c(FT),d(FF),e(FW); if(!a||!b||!c||!d||!e) return false;
030   for(auto&s:beds) a<<s.id<<'|'<<s.name<<'|'<<s.prio<<'|'<<s.intervalMin<<'|'<<s.durationSec<<'|'<<s.nextWater<<'\n';
031   for(auto&s:doses) b<<s.bedId<<'|'<<s.formula<<'|'<<s.ml<<'|'<<s.when<<'\n';
032   for(auto&s:tank)  c<<s.tempC<<'|'<<s.pH<<'|'<<s.nh3<<'|'<<s.no2<<'|'<<s.no3<<'|'<<s.when<<'\n';
033   for(auto&s:feed)  d<<s.tankId<<'|'<<s.type<<'|'<<s.grams<<'|'<<s.when<<'|'<<s.nextWhen<<'\n';
034   for(auto&s:wells) e<<s.hardness<<'|'<<s.ec<<'|'<<s.when<<'\n';
035   return true;
036 }
037 
038 bool loadAll(vector<Bed>& beds,vector<NutrientDose>& doses,vector<TankReading>& tank,vector<FishFeeding>& feed,vector<WellWater>& wells){
039   beds.clear(); doses.clear(); tank.clear(); feed.clear(); wells.clear();
040   auto readLines=[&](string path, auto lambda){
041     ifstream in(path); if(!in) return true; string line;
042     while(getline(in,line)){ if(line.empty()) continue; vector<string> p; string cur; stringstream ss(line); while(getline(ss,cur,'|')) p.push_back(cur); lambda(p); }
043     return true;
044   };
045   readLines(FB,[&](auto&p){ if(p.size()==6){ Bed s{stoi(p[0]),p[1],stoi(p[2]),stoi(p[3]),stoi(p[4]),stoll(p[5])}; beds.push_back(s);} });
046   readLines(FD,[&](auto&p){ if(p.size()==4){ NutrientDose s{stoi(p[0]),p[1],stod(p[2]),stoll(p[3])}; doses.push_back(s);} });
047   readLines(FT,[&](auto&p){ if(p.size()==6){ TankReading s{stod(p[0]),stod(p[1]),stod(p[2]),stod(p[3]),stod(p[4]),stoll(p[5])}; tank.push_back(s);} });
048   readLines(FF,[&](auto&p){ if(p.size()==5){ FishFeeding s{stoi(p[0]),p[1],stod(p[2]),stoll(p[3]),stoll(p[4])}; feed.push_back(s);} });
049   readLines(FW,[&](auto&p){ if(p.size()==3){ WellWater s{stod(p[0]),stod(p[1]),stoll(p[2])}; wells.push_back(s);} });
050   return true;
051 }
052 
053 void addOrUpdateBed(vector<Bed>& beds){
054   cout<<"\n== Add/Update Bed ==\n";
055   int id=readInt("Bed id: ",0); int i=findBed(beds,id);
056   if(i==-1){
057     Bed s; s.id=id; s.name=readLine("Name: "); s.prio=readInt("Priority (0=high): ",0);
058     s.intervalMin=readInt("Water interval (min): ",1); s.durationSec=readInt("Water duration (sec): ",1); s.nextWater=NOW+s.intervalMin;
059     beds.push_back(s); cout<<"Added.\n";
060   }else{
061     Bed& s=beds[i];
062     string nm=readLine("Name (blank=keep): "); if(!nm.empty()) s.name=nm;
063     s.prio=readInt("Priority (0=high): ",0);
064     s.intervalMin=readInt("Water interval (min): ",1);
065     s.durationSec=readInt("Water duration (sec): ",1);
066     s.nextWater=NOW+s.intervalMin;
067     cout<<"Updated.\n";
068   }
069 }
070 
071 void setWatering(vector<Bed>& beds){
072   cout<<"\n== Set Watering ==\n";
073   int id=readInt("Bed id: ",0); int i=findBed(beds,id); if(i==-1){ cout<<"Not found.\n"; return; }
074   beds[i].intervalMin=readInt("Interval (min): ",1);
075   beds[i].durationSec=readInt("Duration (sec): ",1);
076   beds[i].nextWater=NOW+beds[i].intervalMin;
077   cout<<"Set.\n";
078 }
079 
080 void logDose(vector<NutrientDose>& doses,const vector<Bed>& beds){
081   cout<<"\n== Log Nutrient Dose ==\n";
082   int id=readInt("Bed id: ",0); if(findBed(beds,id)==-1){ cout<<"Bed not found.\n"; return; }
083   NutrientDose s{ id, readLine("Formula (e.g., 4-1-2): "), readDouble("Amount (ml): ",0.0), NOW };
084   doses.push_back(s); cout<<"Dose logged.\n";
085 }
086 
087 void logTank(vector<TankReading>& tank){
088   cout<<"\n== Record Fish Tank ==\n";
089   TankReading r{ readDouble("Temp C: "), readDouble("pH: "), readDouble("Ammonia mg/L: ",0.0),
090                  readDouble("Nitrite mg/L: ",0.0), readDouble("Nitrate mg/L: ",0.0), NOW };
091   tank.push_back(r);
092   cout<<fixed<<setprecision(2)<<"Tank @ "<<r.when<<"  T:"<<r.tempC<<"  pH:"<<r.pH<<"  NH3:"<<r.nh3<<"  NO2:"<<r.no2<<"  NO3:"<<r.no3<<"\n";
093   warnTank(r);
094 }
095 
096 void logFeed(vector<FishFeeding>& feed){
097   cout<<"\n== Log Fish Feeding ==\n";
098   int tank=readInt("Tank id: ",1); string type=readLine("Feed type: "); double g=readDouble("Grams: ",0.0); int h=readInt("Hours to next: ",1);
099   feed.push_back(FishFeeding{tank,type,g,NOW,NOW+h*60});
100   cout<<"Feeding logged. Next @ "<<feed.back().nextWhen<<"\n";
101 }
102 
103 void showFeedReminders(vector<FishFeeding> feed){
104   cout<<"\n== Feeding Reminders ==\n";
105   if(feed.empty()){ cout<<"None.\n"; return; }
106   sort(feed.begin(),feed.end(),[](auto&a,auto&b){return a.nextWhen<b.nextWhen;});
107   for(auto&f:feed){ cout<<"Tank "<<f.tankId<<" next @ "<<f.nextWhen<<" (now="<<NOW<<")"<<(f.nextWhen<=NOW?"  <-- DUE":"")<<"\n"; }
108 }
109 
110 void logWell(vector<WellWater>& wells){
111   cout<<"\n== Record Well Water ==\n";
112   WellWater w{ readDouble("Hardness (ppm): ",0.0), readDouble("EC (mS/cm): ",0.0), NOW };
113   wells.push_back(w); cout<<"Well @ "<<w.when<<"  Hardness:"<<w.hardness<<"  EC:"<<w.ec<<"\n";
114   if(w.hardness>250||w.ec>1.0) cout<<"  Note: hard/high EC—dose gently.\n";
115 }
116 
117 void listBeds(vector<Bed>& beds){
118   if(beds.empty()){ cout<<"No beds.\n"; return; }
119   cout<<"Sort by: 1) NextWater  2) Name  (else none)\n";
120   int m=readInt("Choice: ");
121   if(m==1) sort(beds.begin(),beds.end(),[](auto&a,auto&b){return a.nextWater<b.nextWater;});
122   else if(m==2) sort(beds.begin(),beds.end(),[](auto&a,auto&b){return a.name<b.name;});
123   cout<<"\n== Beds ==\n";
124   for(auto&s:beds) cout<<"#"<<s.id<<" "<<s.name<<" prio:"<<s.prio<<" int:"<<s.intervalMin<<" dur:"<<s.durationSec<<" next:"<<s.nextWater<<"\n";
125 }
126 
127 void advanceTime(vector<Bed>& beds){
128   cout<<"\n== Advance Time ==\n";
129   int min=readInt("Minutes: ",1); NOW+=min;
130   for(auto&b:beds) while(b.nextWater<=NOW) b.nextWater+=b.intervalMin;
131   cout<<"Now="<<NOW<<"\n";
132 }
133 
134 void menu(){
135   cout<<"\n=== Pumpkin Patch Aquaponics Controller ===\n";
136   cout<<"Now(min)="<<NOW<<"\n";
137   cout<<"1) Add/Update Bed\n2) Set Watering\n3) Log Nutrient Dose\n4) Record Fish Tank\n5) Log Fish Feeding\n6) Feeding Reminders\n7) Record Well Water\n8) List/Sort Beds\n9) Advance Time\nS) Save  L) Load\nQ) Quit\n";
138 }
139 
140 int main(){
141   ios::sync_with_stdio(false); cin.tie(nullptr);
142   vector<Bed> beds; vector<NutrientDose> doses; vector<TankReading> tank; vector<FishFeeding> feed; vector<WellWater> wells;
143   loadAll(beds,doses,tank,feed,wells);
144   for(;;){
145     menu(); cout<<"Choice: "; string ch; getline(cin,ch); if(ch.empty()) continue; char c=toupper((unsigned char)ch[0]);
146     if(c=='1') addOrUpdateBed(beds);
147     else if(c=='2') setWatering(beds);
148     else if(c=='3') logDose(doses,beds);
149     else if(c=='4') logTank(tank);
150     else if(c=='5') logFeed(feed);
151     else if(c=='6') showFeedReminders(feed);
152     else if(c=='7') logWell(wells);
153     else if(c=='8') listBeds(beds);
154     else if(c=='9') advanceTime(beds);
155     else if(c=='S'){ cout<<(saveAll(beds,doses,tank,feed,wells)?"Saved.\n":"Save failed.\n"); }
156     else if(c=='L'){ cout<<(loadAll(beds,doses,tank,feed,wells)?"Loaded.\n":"Load failed.\n"); }
157     else if(c=='Q'){ saveAll(beds,doses,tank,feed,wells); cout<<"Bye!\n"; break; }
158     else cout<<"?\n";
159   }
160   return 0;
161 }
