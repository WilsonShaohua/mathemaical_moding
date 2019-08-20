#include <iostream>
#include<queue>
#include<vector>
#pragma warning(disable :4996)

using namespace std;
#define STEP_TIME 1 //动车到站时间间隔（单位：分钟）
#define CRH2 2 //CRH2 动车型号
#define CRH3 3 //CRH3 动车型号
#define CRH5 5 //CRH5 动车型号
#define CRH6 6 //CRH6 动车型号
const int arriving_train_numbers = 11; //将要进站动车数量
const int train_models = 4; //动车类型数量
const int numbers = 3; //工序数
const int max_train_model_no = 6; //最大动车类型编号
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
	//根据分钟制时间单位转换为小时制
	void setTime(int _minute) {
		hour = _minute / 60;
		minute = _minute % 60;
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
		printf("  %02d:%02d", hour % 24, minute);
	}
};

//动车结构体 
struct _Train {
	int model; //动车型号
	myTm arrive_time; //动车到达时间 
	int factory_id[numbers]; //工序对应车间id
	myTm working_start_time[numbers]; //工序开始时间
	myTm working_end_time[numbers]; //工序结束时间
	/**
	 *构造函数，初始化动车型号和到站时间和工作时长
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


	//动车维修时间表打印函数
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
	int train_id; //当前车间维修动车id；
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
	/**
	 * 初始化车间
	 */
	int factory_number[numbers] = { 3,8,5 };//车间的数量
	queue<int> free_factory_ids[numbers]; //空闲车间id；
	//初始化三道工序的车间数量，当前所有车间全部处于空闲状态
	for (int i = 0; i < numbers; i++) {
		//构造当前工序车间的id
		for (int j = 1; j <= factory_number[i]; j++) {
			free_factory_ids[i].push(j);
		}
	}
	/**
	 *初始化各工序等待动车队列
	 */
	queue<int> waitting_trains_id[numbers + 1];//等待动车的id

	/**
	  * 初始化各工序各车间维修队列（优先队列）
	  */
	priority_queue<Factory> factories[3]; //三道工序的车间优先队列

	/**
	 * 初始化各类型动车的各道工序的维修时间
	 */
	 //动车型号数组
	int train_model[train_models] = { CRH2,CRH3,CRH5,CRH6 };

	//各型号动车工作时间（分钟制）
	int working_time[train_models][numbers] = {
		{60,120,90}, //CRH2
		{48,144,30}, //CRH3
		{78,150,90}, //CRH5
		{60,162,18}  //CRH6
	};
	/**
	 * 使用简单对应键值关系建立散列表
	 * 散列方程为 动车型号工序时间= 动车型号值*1
	 */
	 //各型号动车工作时间散列表（分钟制）
	int working_time_hash[max_train_model_no + 1][numbers];
	//将各型号动车工作时间存放到对应散列表
	for (int i = 0; i < train_models; i++) {
		for (int j = 0; j < numbers; j++) {
			working_time_hash[train_model[i]][j] = working_time[i][j];
		}
	}

	/**
	  * 初始化即将维修动车的类型、到达时间
	  */
	  //初始化即将到达动车的型号
	int arring_train_model[arriving_train_numbers] = { CRH2,CRH5,CRH2,CRH6,CRH3,CRH6,CRH2,CRH5,CRH3,CRH3,CRH6 };
	//初始话即将到达的动车的时间（分钟制）
	int arriving_train_times_minutes[arriving_train_numbers] = { 16,47,82,120,141,182,211,239,241,267,309 };
	//初始化即将到达的动车的时间（小时制）
	myTm arriving_train_times_hours[arriving_train_numbers];
	//将分钟值时间转化为小时值时间
	for (int i = 0; i < arriving_train_numbers; i++) {
		arriving_train_times_hours[i].setTime(arriving_train_times_minutes[i]);
	}
	vector<_Train> trains; //即将到达的动车数组
	//使用即将到达动车的型号、到达时间、各工序时间初始化即将到达动车数组，
	for (int i = 0; i < arriving_train_numbers; i++) {
		_Train train(arring_train_model[i], arriving_train_times_hours[i]); //创建动车
		trains.push_back(train); //将动车存入队列中
	}

	/**
	 * 初始化时间参数
	 */
	myTm first_arrived_time; //第一辆动车到达时间
	myTm last_arrived_time;//最后一辆动车到达时间
	if (trains.size() > 0) {
		first_arrived_time = trains[0].arrive_time;
		last_arrived_time = trains[trains.size() - 1].arrive_time;
	}
	//当前时间假设为第一辆动车的到达时间
	myTm now_time = first_arrived_time;//当前时间
	//程序开始时间为第一辆动车到达时间
	myTm start_time = last_arrived_time; //程序开始时间
	myTm step_time(0, STEP_TIME);//时间流逝步长为1分钟
	//总计时间消耗
	myTm total_time(0,0);
	int cur = 0; //当前等待加入维修队列的动车
	while (true) {
		/**
		 *判断当前时间大于最后一辆动车的到达时间之
		 *如果所有动车维修工厂工作队列都为空
		 *那么维修工作全部完成
		 *退出程序
		 */
		if (!now_time.cmp(last_arrived_time))//当前时间大于最后一辆动车进入的时间时
		{
			/**
			  * 设置标志allFactoryEmpty bool类型
			  * true:全部维修完毕
			  * false：未全部维修完成
			  * 默认为为true
			  */
			bool allFactoryEmpty = true;
			//遍历全部工厂队列，出现不为空的工作工厂队列则表示维修工作未完成
			for (int i = 0; i < numbers; i++) {
				if (!factories[i].empty()) {
					allFactoryEmpty = false;
					break;
				}
			}
			//如果全部车间为空，车辆维修完毕退出程序
			if (allFactoryEmpty) {
				total_time = total_time-step_time;
				now_time = now_time - step_time;
				break;
			}
		}
		/** 当前时间到达动车进站时间
		  * 该动车进入a工序等待队列
		  * 等待进站动车序号自增1
		  */
		if (cur < trains.size() && now_time.hour == trains[cur].arrive_time.hour && now_time.minute == trains[cur].arrive_time.minute) {
			waitting_trains_id[0].push(cur);
			cur++;
		}


		//for循环numbers次工序
		for (int i = 0; i < numbers; i++) {
			//当前工序车间的队头元素
			Factory f;
			if (!factories[i].empty()) {
				f = factories[i].top();
			}
			//当前车间非空且车间维修车辆的结束时间小于当前时间，则将当前车辆出队并放入下一维修等待队列
			while (!factories[i].empty() && f.end_work_time.cmp(now_time)) {
				Factory factory = factories[i].top(); //获取当前队首元素
				factories[i].pop(); //出队				
				waitting_trains_id[i + 1].push(factory.train_id); //加入下一工序等待环节
				free_factory_ids[i].push(factory.id);//添加进入当前车间队列
				if (!factories[i].empty()) {
					f = factories[i].top();
				}
			}
			//当前工序空闲车间不为空并且当前工序等待动车不为空
			while (!free_factory_ids[i].empty() && !waitting_trains_id[i].empty()) {
				Factory factory;
				factory.id = free_factory_ids[i].front(); //当前车间id来自空闲车间
				free_factory_ids[i].pop();//空闲车间出队一个
				factory.train_id = waitting_trains_id[i].front(); //得到当前等待动车id
				waitting_trains_id[i].pop();//等待动车出队一辆
				trains[factory.train_id].working_start_time[i] = now_time; //添加当前工序结束时间				
				myTm working_time;//当前工序的工作时间
				//当前工序的工作时间来自当前动车类型工作时间散列表中对应的值
				working_time.setTime(working_time_hash[trains[factory.train_id].model][i]);
				trains[factory.train_id].working_end_time[i] = now_time + working_time; //添加当前工序结束时间
				factory.start_work(now_time, working_time); //车间开始工作				
				factories[i].push(factory); //车间进入工作队列
				trains[factory.train_id].factory_id[i] = factory.id; //添加当前工序车间				
			}
		}
		total_time = total_time + step_time; //总消耗时间增加一个动车到达间隔时间
		now_time = now_time + step_time; //当前时间增加一个动车到达间隔时间
	}
	cout << "编号 类别 到达时间 A车间编号 A开始  A结束   B车间编号 B开始  B结束   C车间编号 C开始  C结束" << endl;
	for (int i = 0; i < trains.size(); i++) {
		printf(" %02d", i);
		trains[i].print();
	}
	//total_time = now_time - start_time;
	//打印耗时信息
	cout << "维修所有动车耗时共计：" << total_time.hour << "时" << total_time.minute << "分" << endl;
	system("pause"); //控制台暂停
	return 0;
}