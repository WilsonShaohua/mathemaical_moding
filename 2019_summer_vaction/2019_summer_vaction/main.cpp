#pragma warning (disable:4996)

#include<bits/stdc++.h> //万能头文件vs不支持dev可使用
using namespace std;
#define ARRIVE_TIME 15 //列车到站时间间隔（单位：分钟） 
#define FACTORY_A_TIME 1*60 //工序A消耗时间 （单位:分钟）
#define FACTORY_B_TIME 2*60 //工序B消耗时间 （单位:分钟）
#define FACTORY_C_TIME 90 //工序C消耗时间（单位：分钟） 
#define ALL_TIME 12*60 //工作总时间（单位：分钟）
#define START_TIME 0 //列车到达时间 （单位：分钟）

const int numbers = 3; //工序数

/**
 *自定义时间结构体
 *包含小时和分钟两种属性
 */
struct myTm {
	int hour; //小时
	int minute;//分钟
	/**
	 *初始化函数
	 *参数说明
	 *_hour：属性hour
	 *_minute:属性minute
	 */
	myTm(int _hour, int _minute) :hour(_hour), minute(_minute) {
		hour = hour + minute / 60;
		minute = minute % 60;
	}
	/**
	 *构造函数
	 */
	myTm() {
		hour = 0;
		minute = 0;
	}
	/**
	 * 比较函数
	 * 和参数时间想比较返回较小者
	 */
	bool cmp(const myTm& time) {
		if (hour == time.hour) {
			return minute <= time.minute;
		}
		return hour < time.hour;
	}

	

	/*
	 *重载运算符：+
	 */
	myTm operator+ (const myTm &time1) {
		myTm t(0, 0);
		t.minute = time1.minute + minute;
		t.hour = time1.hour + hour + t.minute / 60;
		t.minute = t.minute % 60;
		return t;
	}

	/*
	 *重载运算符：-
	 */
	myTm operator- (const myTm &time2) {
		myTm t(0, 0);
		t.minute = minute - time2.minute;
		t.hour = hour - time2.hour;
		if (t.minute < 0) {
			t.minute = t.minute + 60;
			t.hour = t.hour - 1;
		}
		return t;
	}

	void print() {
		printf("  %02d:%02d", hour%24, minute);
	}
};

//列车结构体 
struct _Train {
	int model; //列车型号
	myTm arrive_time; //列车到达时间 
	int factory_id[numbers]; //工序对应车间id
	myTm working_start_time[numbers]; //工序开始时间
	myTm working_end_time[numbers]; //工序结束时间
	/**
	 *构造函数，初始化列车型号和到站时间和工作时长
	 */
	_Train(int _model, myTm _arrive_time)
		:model(_model), arrive_time(_arrive_time) {}
	/**
	 *构造函数
	 *初始化到站时间
	 *型号默认为0
	 */
	_Train(myTm _arrive_time) :arrive_time(_arrive_time) {
		model = 0;
	};


	//列车维修时间表打印函数
	void print() {
		cout << "  CRH" << model;
		arrive_time.print();
		for (int i = 0; i < numbers; i++) {
			//cout<<setw(2) << factory_id[i]; 
			printf("      %02d   ", factory_id[i]);
			working_start_time[i].print(); working_end_time[i].print();
		}
		cout << endl;
	}
	//获取当前（cur）工序的工作时间：working_end_time - working_start_time
	myTm get_working_time(int cur) {
		return working_end_time[cur] - working_start_time[cur];
	}
};

//工序结构体 
struct Factory {
	int train_id; //当前车间维修列车id；
	myTm end_work_time;//结束工作的时间
	int id;//工序id； 

	/**
	 *构造函数
	 */
	Factory(int _id) :id(_id) {	}
	Factory() {	}

	//开始工作
	void start_work(myTm now_time, myTm working_time) {
		end_work_time = end_work_time + working_time;
		end_work_time = end_work_time + now_time;
	}

};
bool operator< (Factory f1, Factory f2) {
	return !f1.end_work_time.cmp(f2.end_work_time);
}

int main() {
	int factory_number[numbers] = { 3,8,5 };//车间的数量
	queue<int> free_factory_ids[numbers]; //空闲车间id；
	//使用优先队列
	priority_queue<Factory> factories[3]; //三道工序的车间
	//初始化三道工序的车间数量，当前所有车间全部处于空闲状态
	for (int i = 0; i < numbers; i++) {
		//构造当前工序车间的id
		for (int j = 1; j <= factory_number[i]; j++) {
			free_factory_ids[i].push(j);
		}
	}

	/**
	 *初始化trains队列
	 *将到达的列车加入列车队列中
	 */
	vector<_Train> trains;//列车安排时刻表队列
	queue<int> waitting_trains_id[numbers + 1]; //等待进入下一工序的列车的id
	time_t now = time(0);//获取系统时间，从1900/0/1 0:0:0开始的秒数
	tm* local_time = localtime(&now);//将系统时间转换为本地时间
	myTm now_time(local_time->tm_hour, local_time->tm_min);//将本地时间获取小时和分钟
	myTm end_time(0, ALL_TIME);//车辆停止到达时间
	end_time = end_time + now_time;
	myTm step_time(0, ARRIVE_TIME);//两辆车之间到达间隔时间
	myTm start_time(0, START_TIME);//车辆到达时间的起始时间
	start_time = now_time; //开始时间等于当前时间
	//初始话车间工作时间
	myTm factory_working_time[numbers];
	factory_working_time[0].minute = FACTORY_A_TIME % 60;
	factory_working_time[0].hour = FACTORY_A_TIME / 60;
	factory_working_time[1].minute = FACTORY_B_TIME % 60;
	factory_working_time[1].hour = FACTORY_B_TIME / 60;
	factory_working_time[2].minute = FACTORY_C_TIME % 60;
	factory_working_time[2].hour = FACTORY_C_TIME / 60;


	myTm total_time(0, 0);//总计耗时
	myTm _now = now_time; //当前时间
	

	/**
	 *当前队列不空的情况下继续操作
	 */
	while (true) {

		//当前时间小于列车停止到达时间这继续添加进入列车队列中。并将该车次id加入等待队列1中
		if (_now.cmp(end_time)) {
			_Train train(_now);//当前时间小于车辆停止到达时间，增加新的列车进入维修i队列
			trains.push_back(train);
			waitting_trains_id[0].push(trains.size() - 1); //当前进入列车id进入等待进入工序0队列
		}
		else {//当前时间超过列车到达时间判断所有车间是否全部为空
			bool allFactoryEmpty = true;//设置标志flag为true：全部维修完毕，false：未全部维修完成
			for (int i = 0; i < numbers; i++) {
				if (!factories[i].empty()) {
					allFactoryEmpty = false;
					break;
				}
			}
			//如果全部车间为空，车辆维修完毕退出程序
			if (allFactoryEmpty) {
				break;
			}
		}

		//for循环numbers次工序
		for (int i = 0; i < numbers; i++) {
			//当前工序车间的队头元素
			Factory f;
			if (!factories[i].empty()) {
				f = factories[i].top();
			}
			//当前车间非空且车间维修车辆的结束时间小于当前时间，则将当前车辆出队并放入下一维修等待队列
			while (!factories[i].empty() && f.end_work_time.cmp(_now)) {

				Factory factory = factories[i].top(); //获取当前队首元素
				factories[i].pop(); //出队				
				waitting_trains_id[i + 1].push(factory.train_id); //加入下一工序等待环节
				free_factory_ids[i].push(factory.id);//添加进入当前车间队列
				if (!factories[i].empty()) {
					f = factories[i].top();
				}
			}
			//当前工序空闲车间不为空并且当前工序等待列车不为空
			while (!free_factory_ids[i].empty() && !waitting_trains_id[i].empty()) {
				Factory factory;
				factory.id = free_factory_ids[i].front(); //当前车间id来自空闲车间
				free_factory_ids[i].pop();//空闲车间出队一个
				factory.train_id = waitting_trains_id[i].front(); //得到当前等待列车id
				waitting_trains_id[i].pop();//等待列车出队一辆
				trains[factory.train_id].working_start_time[i] = _now; //添加当前工序结束时间
				trains[factory.train_id].working_end_time[i] = _now + factory_working_time[i]; //添加当前工序结束时间
				factory.start_work(_now, factory_working_time[i]); //车间开始工作				
				factories[i].push(factory); //车间进入工作队列
				trains[factory.train_id].factory_id[i] = factory.id; //添加当前工序车间				
			}
		}
		//total_time = total_time + step_time; //总消耗时间增加一个列车到达间隔时间
		_now = _now + step_time; //当前时间增加一个列车到达间隔时间
	}
	cout << "编号 型号 到达时间 A车间编号 A开始 A结束 B车间编号 B开始 B结束 C车间编号 C开始 C结束" << endl;
	for (int i = 0; i < trains.size(); i++) {
		printf(" %02d", i);
		trains[i].print();
	}
	total_time = _now - start_time; //总时间为最后一辆列车检修完成时间（当前时间）时间减去列车发车时间
	//打印耗时信息
	cout << "维修所有列车耗时共计：" << total_time.hour << "时" << total_time.minute << "分" << endl;
	system("pause"); //控制台站厅
	return 0;
}