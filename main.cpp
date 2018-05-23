
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<string.h>
#include<math.h>
#include<time.h>
#include<map>
#include<algorithm>
#include<stdlib.h>
#include<exception>
using namespace std;
#define normal
#undef normal

#define SIZE 100
#define GROUP_SIZE 100  // 种群规模
#define INF 999999
#define maxInter 6000  // 总迭代次数
#define PMUTATION 0.1  //突变率
#define crossProbability 0.9
int craneSize; //岸桥数量
#define maxLine 3000//岸线长度最大值
//#define maxCrane 5 //单个船使用的最大岸桥数量
#define INT_MAX ((int)(~0U>>1))  
#define INT_MIN (-INT_MAX - 1) 
#define X1 6000 //未完成工作量的耗费代价
int asalary[4];
int esalary[4];
int workefficiency[24];
int craneden[10];
int T, L, N;//港口服务时长，港口长度，停船的数量。

int best_pos, best_f;
FILE *fpdraw,*bestplan;




typedef struct Boat{
	int arritime;//船到达时间
	long long leavetime;//船规定离开的截止时间
	int workload;//工作量
	int boatwidth;//船宽度
	int goodlocation;//偏好的船停泊位置
	int bertlocation;//船停泊位置
	int starttime;//实际停靠的时间
	int endtime;//实际离开的时间
	int ID;
	bool find;
	int boatMaxCrane;//单个船只使用的最大岸桥数量
	vector<int> percrane;//卸货时候每小时岸桥数量
	Boat() {

	}

} Boat;
void cpBoat(const Boat src[], Boat dst[]);
typedef struct allcost {
	long long cost;
	long long costT;
	long long costS;
	bool allfind;
	allcost() :cost(0), costS(0), costT(0),allfind(true) {

	}
	allcost(const allcost& s) {
		cost = s.cost;
		costS = s.costS;
		costT = s.costT;
		allfind = s.allfind;
	}
	bool operator <(const allcost& s) const{
		if (cost < s.cost) {
			return true;
		}
		return false;
	}
	bool operator >(const allcost& s) const{
		if (cost > s.cost) {
			return true;
		}
		return false;
	}
}allcost;
typedef struct Group{
	int order[SIZE];
	Boat boat[SIZE];
	Group() {

	}
	allcost f;
	bool operator <(const Group& s) const {
		if (f < s.f) {
			return true;
		}
		else
		{
			return false;
		}
	}

	Group(const Group& s) {
		cpBoat(s.boat, boat);
		for (int i = 0; i < SIZE; i++) {
			order[i] = s.order[i];
		}
		f = s.f;
	}
}Group;
bool isSame(Group&a, Group& b) {
	if (a.f.cost != b.f.cost || a.f.costS!=b.f.costS || a.f.costT!=b.f.costT)
		return false;
	return true;
}

int cmp(Group& a,  Group& b) {
	if (a.f > b.f)
		return 1;
	else
	{
		return 0;
	}
}
typedef struct Record {
	int start;
	int end;
	int bertlocation;
	vector<int> percrane;
	bool find;
	allcost cost;
	
	Record() :find(false) {

	}
	Record(const Record& s) {
		this->start = s.start;
		this->bertlocation = s.bertlocation;
		this->cost = s.cost;
		this->end = s.end;
		this->find = s.find;
		this->percrane.clear();
		for (int i = 0; i < s.percrane.size(); i++) {
			this->percrane.push_back(s.percrane[i]);
		}
	}
}Record;



void cpBoat(const Boat src[], Boat dst[]) {
	for (int i = 0; i < N; i++) {
		dst[i].arritime = src[i].arritime;
		dst[i].leavetime = src[i].leavetime;
		dst[i].workload = src[i].workload;
		dst[i].boatwidth = src[i].boatwidth;
		dst[i].bertlocation = src[i].bertlocation;
		dst[i].starttime = src[i].starttime;
		dst[i].endtime = src[i].endtime;
		dst[i].goodlocation = src[i].goodlocation;
		dst[i].ID = src[i].ID;
		dst[i].find = src[i].find;
		dst[i].boatMaxCrane = src[i].boatMaxCrane;
		dst[i].percrane.clear();
		for (int j = 0; j < src[i].percrane.size(); j++) {
			dst[i].percrane.push_back(src[i].percrane[j]);
		}

	}
}

void cpOneBoat(Boat& src, Boat& dst) {
	dst.arritime = src.arritime;
	dst.leavetime = src.leavetime;
	dst.workload = src.workload;
	dst.boatwidth = src.boatwidth;
	dst.bertlocation = src.bertlocation;
	dst.starttime = src.starttime;
	dst.endtime = src.endtime;
	dst.goodlocation = src.goodlocation;
	dst.ID = src.ID;
	dst.find = src.find;
	dst.boatMaxCrane = src.boatMaxCrane;
	dst.percrane.clear();
	for (int j = 0; j < src.percrane.size(); j++) {
		dst.percrane.push_back(src.percrane[j]);
	}
}

void swapBoat(Boat& a, Boat& b) {
	Boat tmp;
	cpOneBoat(a, tmp);
	cpOneBoat(b, a);
	cpOneBoat(tmp, b);
}

void swapGroup(Group& a, Group& b) {
	for (int i = 0; i < N; i++) {
		swap(a.order[i], b.order[i]);
		swapBoat(a.boat[i], b.boat[i]);
	}
	swap(a.f, b.f);
	
}
int mp_crane[500];//mp_crane[t]表示t时间时的剩余空闲岸桥数
bool mp_bertlocation[500][maxLine];//停泊位置和时间的二维占用图
Boat a[SIZE];
Boat a_copy[SIZE];
Group g[GROUP_SIZE];
Group children;
int mp_cranesrc[200];
void init_cranesrc(){
	for (int i = 0; i < 200; i++) {

#ifndef normal
		int index = i%24;
		if(index<6 || index>17){
			int tmp= rand() % (craneSize/2);
			mp_cranesrc[i]=craneSize/2+1+tmp;
		}
		else
		{
			mp_cranesrc[i]=craneSize;
		}
#else
		mp_cranesrc[i] = craneSize;
#endif
		//cout<<mp_cranesrc[i]<<" ";
		//mp_crane[i] = (index > 5 && index < 18) ? craneSize:craneSize/2+2;		
	}
}


void init() {
	//memset(mp_crane, false, sizeof(mp_crane));
	memset(mp_bertlocation, 0, sizeof(mp_bertlocation));
	for (int i = 0; i < 200; i++) {
		mp_crane[i]=mp_cranesrc[i];
		//mp_crane[i] = (index > 5 && index < 18) ? craneSize:craneSize/2+2;		
	}
	
	
}

bool _in(int x, int y) {
	return (x < T && y < L);
}
void fill(int x1, int x2, int y1, int y2) {
	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			mp_bertlocation[i][j] = true;
		}
	}
}
void random_N(int a[]) {
	for (int i = N - 1; i > 0; i--) {
		int index = rand() % N;
		swap(a[i], a[index]);
	}
}
bool isEmpty(Boat& a, int t, int l) {
	for (int i = l; i < l + a.boatwidth; i++)
	{
		if (mp_bertlocation[t][i] == true)
			return false;
	}
	return true;
}
Record endTime(Boat& a, int t, int l) {
	int load = 0;
	long long end = t;
	Record re;
	re.bertlocation = l;
	int maxCrane = a.boatMaxCrane;
	
	while (true)
	{
		if (!isEmpty(a, end, l)) {
			break;
		}
		
		int tmp_time = end % 24;
		int cur_efficiency = -1;
		/*if (tmp_time >= 6 && tmp_time < 18) {
			cur_efficiency = 30;
		}
		else
		{
			cur_efficiency = 15;
			maxCrane = craneden[maxCrane];
		}*/
		cur_efficiency = workefficiency[tmp_time];
		if(tmp_time < 6 && tmp_time > 17)
		{
			maxCrane = craneden[a.boatMaxCrane];
		}		
		//	cout<<a.boatMaxCrane<<" "<<maxCrane<<endl;
		 //workefficiency[tmp_time];
		//maxCrane = maxCrane >> craneden[tmp_time];
		//cout << workefficiency[tmp_time] << endl;
		//int cur_efficiency = 30;
		if (mp_crane[end] >= maxCrane) {
			
			int tmpload = maxCrane * cur_efficiency;
			if (tmpload >= (a.workload-load))
			{	
				re.percrane.push_back(ceil(((double)(a.workload-load))/(double)cur_efficiency));
				load = a.workload;
				re.find = true;
				end++;
				break;				
			}
			else
			{
				load += tmpload;
				re.percrane.push_back(maxCrane);

			}
		}
		else
		{
			int tmpload = mp_crane[end] * cur_efficiency;
			if (tmpload >= (a.workload - load))
			{				
				re.percrane.push_back(ceil(((double)(a.workload - load)) / (double)(cur_efficiency)));
				load = a.workload;
				re.find = true;
				end++;
				break;
			}
			else
			{
				load += tmpload;
				re.percrane.push_back(mp_crane[end]);

			}
		}
		end++;
		if (end > T) {
			break;
		}
	}
	if (re.find) {
		re.start = t;
		re.end = end;
		re.cost.costT = (end - a.leavetime);
		re.cost.costT = re.cost.costT > 0 ? re.cost.costT : 0;
		//cout << re.cost.costT << "   "<<re.cost.cost << endl;
		re.cost.cost += (re.cost.costT+100)*35;
		//32
		
		
		//re.cost += abs(l - a.goodlocation);
		for (int i = t; i < end; i++) {
			int s = i % 24;
			if (s >= 0 && s <= 5) {
				re.cost.costS += re.percrane[i - t] * asalary[0];
				//re.cost += re.percrane[i - t] * esalary[0];
			}
			else if (s>=6 && s<=11) {
				re.cost.costS += re.percrane[i - t] * asalary[1];
				//re.cost += re.percrane[i - t] * esalary[1];
			}
			else if (s>=12 && s<=17) {
				re.cost.costS += re.percrane[i - t] * asalary[2];
				//re.cost += re.percrane[i - t] * esalary[2];
			}
			else
			{
				re.cost.costS += re.percrane[i - t] * asalary[3];
				//re.cost += re.percrane[i - t] * esalary[3];
			}
		}
		re.cost.cost += re.cost.costS;
	}
	else
	{
		re.cost.cost = INF;
	}
	return re;
}

allcost Greedy(Boat a[])//贪心摆放
{
	init();
	allcost gcost;
	for (int i = 0; i < N; i++) {
		//if (a[i].ID == 4) {
			//cout << "kaishi" << endl;
		//}
		bool find = false;
		Record re;
	//	cout<<"for xunhuan前";
		for (int t = a[i].arritime; t < T; t++) {
			int maxl = max((L - a[i].boatwidth - a[i].goodlocation), a[i].goodlocation);
			if (mp_crane[t] == 0)
				continue;
			for (int l = 0; l < maxl; l++) {
				int l1 = a[i].goodlocation + l;
				int l2 = a[i].goodlocation - l;
				if (l1 < L - a[i].boatwidth) {
					Record tmp = endTime(a[i], t, l1);
					if (tmp.find) {
						//cout << "boat ID:" << a[i].ID << " ";
						//cout << "cost" << tmp.cost << endl;
						if (!re.find)
							re = tmp;
						else
						{
							if (re.cost > tmp.cost) {
								re = tmp;
							}
						}
						
					}
				}
				if (l2 >= 0) {
					Record tmp = endTime(a[i], t, l2);
					if (tmp.find) {
						//cout << "boat ID:" << a[i].ID << " ";
						//cout << "cost" << tmp.cost << endl;
						if (!re.find)
							re = tmp;
						else
						{
							if (re.cost > tmp.cost) {
								re = tmp;
							}
						}
						
					}
				}
				//if (re.find)
					//break;
			}
			if (re.find) {
				break;
			}
		}
		if (re.find)
		{
			//cout << gcost.cost << " " << re.cost.cost << endl;
			gcost.cost += re.cost.cost;
			gcost.costS += re.cost.costS;
			gcost.costT += re.cost.costT;
			a[i].bertlocation = re.bertlocation;
			a[i].endtime = re.end;
			a[i].starttime = re.start;
			a[i].percrane = re.percrane;
			//cout << "boat ID:" << a[i].ID << ";" <<"boat width:"<<a[i].boatwidth<<endl;
			for (int j = re.start; j < re.end; j++) {
				//cout << "时间：" << j << " ";
				for (int x = re.bertlocation; x < (re.bertlocation + a[i].boatwidth); x++) {
					mp_bertlocation[j][x] = true;
				}
				mp_crane[j] -= re.percrane[j - re.start];
				//cout << "usedcranesize:" << re.percrane[j - re.start] << endl;
			}
			a[i].find = true;
			//cout << endl;
		}			
		else
		{
			a[i].find = false;
			gcost.allfind = false;
			gcost.cost += X1;
			//cout << "boat ID:" << a[i].ID << "没有合适的位子" << endl;
		}
	}
	return gcost;
};

//交叉活动
void cross(int start_pos, int end_pos, int parent1[], int parent2[]) {
	bool used[SIZE];
	memset(used, false, sizeof(used));
	for (int i = 0; i < N; i++) {
		children.order[i] = 0;
	}
	if (start_pos > end_pos) {
		swap(start_pos, end_pos);
	}
	for (int i = start_pos; i <= end_pos; i++) {
		children.order[i] = parent1[i];
		used[parent1[i]] = true;
	}
	for (int i = 0; i < N; i++) {
		if (i >= start_pos&&i <= end_pos)
			continue;
		for (int j = 0; j < N; j++) {
			if (!used[parent2[j]]) {
				children.order[i] = parent2[j];
				used[parent2[j]] = true;
				break;
			}
		}
	}
	for (int i = 0; i < N; i++) {
		cpOneBoat(a[children.order[i]], children.boat[i]);
	}

}

FILE *in;

void collectData(const Group& s, FILE* plan){
	int boatnum[200];
	int maxday=-1;
	for(int i=0;i<200;i++){
		boatnum[i]=0;
	}
	for(int i=0; i<N; i++){
		const Boat* a = &(s.boat[i]);
		int start = a->starttime;
		int end = a->endtime-1;
		if(end>maxday)
		{
			maxday=end;
		}
		for(int j=start; j<=end; j++){
			boatnum[j]+=a->percrane[j -start];
		}
	}
	//cout<<"每天所有船总共使用的岸桥数分布：  ";
	fprintf(plan,"%s","每天所有船总共使用的岸桥数分布：  ");
	for(int i=0;i<maxday;i++){
		//cout<<boatnum[i]<<" ";
		fprintf(plan,"%d%s",boatnum[i]," ");
	}
	//cout<<endl;
	fprintf(plan,"\n");
}

int preMeanf = -1;
int preMeanT = -1;
int preMeanS = -1;
void writePlan(const Group& a) {
        fprintf(bestplan, "%s%s%s\n", "*********************", "第x种方案**************************");
        for (int i = 0; i < N; i++) {
                const Boat* s = &(a.boat[i]);
                int id = s->ID + 1;
                fprintf(bestplan, "%s%d\n", "船ID:", id);
                if (!s->find) {
                        cout << "no find" << endl;
                        fprintf(bestplan, "%s\n", "找不到方案");
                        continue;
                }
                fprintf(bestplan, "%s%d%s%d\n", "船宽:", s->boatwidth, ";  停靠位置:", s->bertlocation);
                int end = s->endtime - 1;
                fprintf(bestplan, "%s%d%s%d\n", "开始时间:", s->starttime, ";   结束时间:", end);
                fprintf(bestplan, "%s", "每天使用的岸桥数:");
                for (int j = s->starttime; j < s->endtime; j++) {
                        fprintf(bestplan, "%d%s", s->percrane[j - s->starttime], " ");
                }
                fprintf(bestplan, "\n");

        }
        //cout << "总的耗费值是" << a.f.cost << endl;^M
        fprintf(bestplan, "%s%d\n", "总的耗费值是：",a.f.cost);
        fprintf(bestplan, "%s%d\n", "时间耗费值是：", a.f.costT);
        fprintf(bestplan, "%s%d\n", "工资耗费值是：", a.f.costS);
        collectData(a,bestplan);
        fprintf(bestplan, "%s\n", "**************************************");

}





int main(int argc,char* argv[]) {
//港口服务总时长，船的数量，港口总长度
	    T = atoi(argv[6]);
        N = atoi(argv[7]);
        L = atoi(argv[8]);
	craneSize = atoi(argv[9]);
	double dResult;
	clock_t lBefore = clock();
	
	if(atoi(argv[2])==100){
		#undef normal
	}
	int seed = (unsigned)time(NULL);
	srand(seed);
	init_cranesrc();
	init();
	fpdraw = fopen( argv[3], "w+");
	if (fpdraw == NULL) {
		cout << "create draw.txt fails" << endl;
		return 0;
	}
	bestplan = fopen(argv[4], "w+");
	if (bestplan == NULL) {
		cout << "create plan.txt fails" << endl;
		return 0;
	}
#ifndef normal
	int tmp_salary[] = { 100,50,50,100 };
	tmp_salary[1]=atoi(argv[2]);
	tmp_salary[2]=atoi(argv[2]);
	tmp_salary[0]=atoi(argv[5]);
	tmp_salary[3]=atoi(argv[5]);

#else
	int tmp_salary[] = { 80,80,80,80 };
#endif // !normal

	
	for (int i = 0; i < 4; i++) {
		asalary[i] = tmp_salary[i];
		esalary[i] = tmp_salary[i];
	}
	for (int i = 0; i < 6; i++) {
#ifndef normal
		workefficiency[i] = 20;
#else
		workefficiency[i] = 30;
#endif // !normal
		//craneden[i] = 1;
	}
	for (int i =6 ; i < 18; i++) {
		workefficiency[i] = 30;
		//craneden[i] = 0;
	}
	for (int i = 18; i < 24; i++) {
#ifndef normal
		workefficiency[i] = 20;
#else
		workefficiency[i] = 30;
#endif // !normal
		//craneden[i] = 1;
	}
	for (int i = 0; i < 10; i++) {
#ifndef normal
		craneden[i] = i;
#else
		craneden[i] = i - 1;
#endif // !normal
	}
	craneden[0] = 0;
	

	
	in=fopen(argv[1], "r");
	if (in==NULL) {
		cout << "no find in.txt" << endl;
		return 0;
	}
	for (int i = 0; i < N; i++) {
		a[i].ID = i;
		//输入船的信息：到达时间，船宽，偏好的停泊位置，船的离开时间，船的卸货负载量,最大作业岸桥数量。
		
		//cin >> a[i].arritime >> a[i].goodlocation >> a[i].workload >> a[i].boatwidth >>  a[i].leavetime >> a[i].boatMaxCrane;
		fscanf(in, "%d %d %d %d %d %d\n", &(a[i].arritime), &(a[i].goodlocation), &(a[i].workload), &(a[i].boatwidth), &(a[i].leavetime), &(a[i].boatMaxCrane));
		
	}

	//初始化种群
	for (int i = 0; i < GROUP_SIZE; i++) {
		//cout << "kaishi" << endl;
		cpBoat(a, a_copy);
		int a_tmp[SIZE] = { 0, };
		for (int j = 0; j < N; j++) {
			a_tmp[j] = a_copy[j].ID;
		}
		/*cout << "原始排序";
		for (int j = 0; j < N; j++) {
			 cout<< a_tmp[j] << " ";
		}
		cout << endl;*/
		random_N(a_tmp);
		/*cout << "随机排序";
		for (int j = 0; j < N; j++) {
			cout<< a_tmp[j] << " ";
		}
		cout << endl;*/
		for (int j = 0; j < N; j++) {
			g[i].order[j] = a_tmp[j];
			cpOneBoat(a_copy[a_tmp[j]],g[i].boat[j]);			
		}
		//cout<<"greedy 前"<<endl;
		try{
			g[i].f=Greedy(g[i].boat);
		}
		catch(exception& e){
			cout<<e.what()<<endl;
			cout<<"greedy chuwenti"<<endl;
		}
		//cout<<"greedy hou"<<endl;
		cout << "第i个种群 " << i << ": cost";
		cout << g[i].f.cost << " " << g[i].f.costS << " " << g[i].f.costT << endl;
		if(g[i].f.allfind == false){
			cout<<"有船找不到方案"<<endl;
			Group& tmp=g[i];
			for(int j=0; j<N; j++){
				const Boat* s = &(tmp.boat[j]);
				if(!s->find){
					cout<<s->ID<<"  no find"<<endl;
				}
			}
		}
		cout << "********************************" << endl;
	}

	//轮盘分配概率（首先计算种群适应度，个体适应度，给每个个体分配轮盘概率）
	for (int ti = 1; ti <= maxInter; ti++) 
	{
		int sum_f = 0;
		int fitness[GROUP_SIZE] = { 0, };//种群个体适应度值
		double p[GROUP_SIZE] = { 0.0, };//种群个体适应度概率
		map<double, int>pie;//每个个体的累积概率组成的轮转概率
		pie.clear();
		for (int i = 0; i < GROUP_SIZE; i++) {
			sum_f += g[i].f.cost;
		}
		for (int i = 0; i < GROUP_SIZE; i++) {
			fitness[i] = sum_f - g[i].f.cost;//因为当前个体的g[i]的f越大，证明此方法越不好，应该给与的概率越小
			p[i] = 1.0*fitness[i] / sum_f;
		}
		double ret = 0.0;
		for (int i = 0; i < GROUP_SIZE; i++) {
			ret += p[i];
			//pie[ret] = i;
		}
		double rate = 0.0;
		for (int i = 0; i < GROUP_SIZE; i++) {			
			rate += p[i] / ret;
			pie[rate] = i;
		}
		int number1, number2;
		do
		{
			double p1 = (double)rand() / RAND_MAX;
			double p2 = (double)rand() / RAND_MAX;
			number1 = pie.lower_bound(p1)->second;//找出概率比较大的父代
			number2 = pie.lower_bound(p2)->second;//找出概率比较大的父代
		} while (number1==number2);

		int number3 = rand() % N;
		int number4 = rand() % N;
		if(crossProbability>((double)rand() / RAND_MAX))
			cross(number3, number4, g[number1].order, g[number2].order);//交叉产生子代。
		
		if (PMUTATION > ((double)rand() / RAND_MAX)) {
			for (int k = 1; k <= 4; k++) {
				int i1 = rand() % N;
				int i2 = rand() % N;
				swapBoat(children.boat[i1], children.boat[i2]);
				swap(children.order[i1], children.order[i2]);
			}
		}
		children.f = Greedy(children.boat);
		if (children.f.allfind == false) {
			continue;
		}
		//算出子代后更新种群，选择种群中表现最差的个体，进行交换。
		int worst_f = INT_MIN;
		int worst_pos = -1;
		best_f = INT_MAX;
		best_pos = -1;
		for (int i = 0; i < GROUP_SIZE; i++) {//寻找内部适应度最差个体
			//cout << i << " " << g[i].f << endl;
			if (g[i].f.cost > worst_f) {
				worst_f = g[i].f.cost;
				worst_pos = i;
			}
			if (g[i].f.cost < best_f) {
				best_f = g[i].f.cost;
				best_pos = i;
			}
		}
		Group cp=children;
		if (children.f.cost < worst_f) {
			swapGroup(children, g[worst_pos]);
		}
		sort(g, g + GROUP_SIZE);
		int meanf = 0;
		int meanfT = 0;
		int meanfS = 0;
		for (int i = 0; i < 10; i++) {
			meanf += g[i].f.cost;
			meanfT += g[i].f.costT;
			meanfS += g[i].f.costS;

		}
		meanf = meanf/10;
		meanfT = meanfT/10;
		meanfS = meanfS/10;
		fprintf(fpdraw, "%d,%d,%d\n", meanf, meanfT, meanfS);
		if (meanf == preMeanf && meanfT == preMeanT && meanfS == preMeanS)
			continue;	
		preMeanf = meanf;
		preMeanT = meanfT;
		preMeanS = meanfS;
		cout << "迭代次数：" << ti << "加权适应值:" << meanf << ";时间适应值:" << meanfT << ";工资适应值:" << meanfS << endl;			
		writePlan(cp);
		//cout << "迭代次数：" << ti << "适应值" << g[0].f << endl;
	}
	//best_pos = 0;
	int count = 0;
	for (int index = 0; index <GROUP_SIZE; index++) {
		if (index != 0) {
			if (isSame(g[index], g[index - 1])) {
				continue;
			}			
		}
		if (count >= 100) {
			break;
		}
		cout << "*********************";
		int order = count + 1;
		fprintf(bestplan, "%s%s%d%s\n", "*********************", "第", order, "种方案**************************");
		cout << "第" << order << "种方案" << "**************************" << endl;
		for (int i = 0; i < N; i++) {
			Boat* s = &(g[index].boat[i]);
			cout << "boat ID:" << s->ID << ";";
			int id = s->ID + 1;
			fprintf(bestplan, "%s%d\n", "船ID:", id);
			if (!s->find) {
				cout << "no find" << endl;
				fprintf(bestplan, "%s\n", "找不到方案");
				continue;
			}
			cout << "boat width:" << s->boatwidth << ";stoplocation:" << s->bertlocation << ";goodbertlocation:"<<s->goodlocation << endl;
			cout << "starttime:" << s->starttime << ";endtime:" << s->endtime << endl;
			fprintf(bestplan, "%s%d%s%d\n", "船宽:", s->boatwidth, ";  停靠位置:", s->bertlocation);
			int end = s->endtime - 1;
			fprintf(bestplan, "%s%d%s%d\n", "开始时间:", s->starttime, ";   结束时间:", end);
			cout << "usedcranesize:";
			fprintf(bestplan, "%s", "每天使用的岸桥数:");
			for (int j = s->starttime; j < s->endtime; j++) {
				cout << s->percrane[j - s->starttime] << " " << endl;
				fprintf(bestplan, "%d%s", s->percrane[j - s->starttime], " ");
			}
			fprintf(bestplan, "\n");
			
			cout << "****************" << endl;

		}
		collectData(g[index],bestplan);
		cout << "总的耗费值是" <<g[index].f.cost<<endl;
		fprintf(bestplan, "%s%d\n", "总的耗费值是：", g[index].f.cost);
		fprintf(bestplan, "%s%d\n", "时间耗费值是：", g[index].f.costT);
		fprintf(bestplan, "%s%d\n", "工资耗费值是：", g[index].f.costS);
		fprintf(bestplan, "%s\n", "**************************************");
		count++;
	}
	dResult = (double)(clock()-lBefore)/CLOCKS_PER_SEC;
	fprintf(bestplan,"%s%lf","总花费：",dResult);	
	if(fpdraw)
		fclose(fpdraw);
	if(bestplan)
		fclose(bestplan);
	if(in)
		fclose(in);
}
