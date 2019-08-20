#include<stdio.h>
#include<iostream>
#include<vector>
#include<queue>
#pragma warning(disable :4996)
using namespace std;
#define STEP_TIME 1 //������վʱ��������λ�����ӣ�
#define CRH2 2 //CRH2 �����ͺ�
#define CRH3 3 //CRH3 �����ͺ�
#define CRH5 5 //CRH5 �����ͺ�
#define CRH6 6 //CRH6 �����ͺ�
const int arriving_train_numbers = 7; //��Ҫ��վ��������
const int train_models = 4; //������������
const int numbers = 5; //������
const int max_train_model_no = 6; //��󶯳����ͱ��
const int maintenance_level_numbers = 5; //ά�޼�������
/**
 *�Զ���ʱ��ṹ��
 *����Сʱ�ͷ�����������
 */
struct myTm {
	int hour; //Сʱ
	int minute;//����
	/**
	 *��ʼ������
	 *����˵��
	 *_hour������hour
	 *_minute:����minute
	 */
	myTm(int _hour, int _minute) :hour(_hour), minute(_minute) {
		hour = hour + minute / 60;
		minute = minute % 60;
	}
	/**
	 *���캯��
	 */
	myTm() {
		hour = 0;
		minute = 0;
	}
	/**
	 * �ȽϺ���
	 * �Ͳ���ʱ����ȽϷ��ؽ�С��
	 */
	bool cmp(const myTm& time) {
		if (hour == time.hour) {
			return minute <= time.minute;
		}
		return hour < time.hour;
	}
	//���ݷ�����ʱ�䵥λת��ΪСʱ��
	void setTime(int _minute) {
		hour = _minute / 60;
		minute = _minute % 60;
	}
	/*
	 *�����������+
	 */
	myTm operator+ (const myTm &time1) {
		myTm t(0, 0);
		t.minute = time1.minute + minute;
		t.hour = time1.hour + hour + t.minute / 60;
		t.minute = t.minute % 60;
		return t;
	}

	/*
	 *�����������-
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

//�����ṹ�� 
struct _Train {
	int model; //�����ͺ�
	myTm arrive_time; //��������ʱ�� 
	int factory_id[numbers]; //�����Ӧ����id
	myTm working_start_time[numbers]; //����ʼʱ��
	myTm working_end_time[numbers]; //�������ʱ��
	/**
	 *���캯������ʼ�������ͺź͵�վʱ��͹���ʱ��
	 */
	_Train(int _model, myTm _arrive_time)
		:model(_model), arrive_time(_arrive_time) {
		memset(factory_id, -1, sizeof factory_id);
	}
	/**
	 *���캯��
	 *��ʼ����վʱ��
	 *�ͺ�Ĭ��Ϊ0
	 */
	_Train(myTm _arrive_time) :arrive_time(_arrive_time) {
		model = 0;
		memset(factory_id, -1, sizeof factory_id);
	};


	//����ά��ʱ����ӡ����
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
	//��ȡ��ǰ��cur������Ĺ���ʱ�䣺working_end_time - working_start_time
	myTm get_working_time(int cur) {
		return working_end_time[cur] - working_start_time[cur];
	}
};


//����ṹ�� 
struct Factory {
	int train_id; //��ǰ����ά�޶���id��
	myTm end_work_time;//����������ʱ��
	int id;//����id�� 

	/**
	 *���캯��
	 */
	Factory(int _id) :id(_id) {	}
	Factory() {	}

	//��ʼ����
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
	 * ��ʼ������
	 */
	int factory_number[numbers] = { 3,8,5,3,2 };//���������
	queue<int> free_factory_ids[numbers]; //���г���id��
	//��ʼ����������ĳ�����������ǰ���г���ȫ�����ڿ���״̬
	for (int i = 0; i < numbers; i++) {
		//���쵱ǰ���򳵼��id
		for (int j = 1; j <= factory_number[i]; j++) {
			free_factory_ids[i].push(j);
		}
	}
	/**
	 *��ʼ��������ȴ���������
	 */
	queue<int> waitting_trains_id[numbers + 1];//�ȴ�������id

	/**
	  * ��ʼ�������������ά�޶��У����ȶ��У�
	  */
	priority_queue<Factory> factories[numbers]; //����ĳ������ȶ���

	/**
	 * ��ʼ�������Ͷ����ĸ��������ά��ʱ��
	 */
	 //�����ͺ�����
	int train_model[train_models] = { CRH2,CRH3,CRH5,CRH6 };

	//���ͺŶ�������ʱ�䣨�����ƣ�
	int working_time[train_models][numbers] = {
		{60,120,90,240,420}, //CRH2
		{48,144,30,288,390}, //CRH3
		{78,150,90,180,390}, //CRH5
		{60,162,18,300,420}  //CRH6
	};
	/**
	 * ʹ�ü򵥶�Ӧ��ֵ��ϵ����ɢ�б�
	 * ɢ�з���Ϊ �����ͺŹ���ʱ��= �����ͺ�ֵ*1
	 */
	 //���ͺŶ�������ʱ��ɢ�б������ƣ�
	int working_time_hash[max_train_model_no + 1][numbers];
	//�����ͺŶ�������ʱ���ŵ���Ӧɢ�б�
	for (int i = 0; i < train_models; i++) {
		for (int j = 0; j < numbers; j++) {
			working_time_hash[train_model[i]][j] = working_time[i][j];
		}
	}
	/**������ͬά�޼�����֮��˳���ϵ
	  * ��ά�޼�����Ϣ�洢��һ����ά������
	  * �У���ʾ��ͬ�ļ�������ж�Ӧά�޼���I����һ�ж�Ӧά�޼������������������4�ж�Ӧά�޼���V
	  * �У���ʾά�޹��򣬵�һ�б�ʾ����a���ڶ��б�ʾ����b�����������������ж�Ӧ����e��
	  * ÿ��ÿ�е�ֵ��ʼ��Ϊ-1��
	  * ����i�У���j�е�ֵָ����һ��������±꣬������ָ���Ӧά�޼���ĵ�һά�޹���
	  * ĳһ�е�ֵΪĬ��ֵʱ����ʾ�ù����ڵ�ǰά�޼����л��ڵ�ǰά�޼����е����һ��ά�޹���
	  */
	int maintenance_level[maintenance_level_numbers][numbers + 1]{
		{1,2,-1,-1,-1,-1},
		{1,2,3,-1,-1,-1},
		{1,2,4,-1,-1,-1},
		{1,3,-1,4,5,-1},
		{1,2,3,4,5,-1}
	};
	/**
	  * ��ʼ������ά�޶��������͡�����ʱ��
	  */
	  //��ʼ���������ﶯ�����ͺ�
	int arring_train_model[arriving_train_numbers] = { CRH2,CRH5,CRH2,CRH6,CRH3,CRH6,CRH2 };
	//��ʼ����������Ķ�����ʱ�䣨�����ƣ�
	int arriving_train_times_minutes[arriving_train_numbers] = { 16,47,82,120,141,182,211 };
	//��ʼ����������Ķ�����ʱ�䣨Сʱ�ƣ�
	myTm arriving_train_times_hours[arriving_train_numbers];
	//������ֵʱ��ת��ΪСʱֵʱ��
	for (int i = 0; i < arriving_train_numbers; i++) {
		arriving_train_times_hours[i].setTime(arriving_train_times_minutes[i]);
	}
	vector<_Train> trains; //��������Ķ�������
	//ʹ�ü������ﶯ�����ͺš�����ʱ�䡢������ʱ���ʼ���������ﶯ�����飬
	for (int i = 0; i < arriving_train_numbers; i++) {
		_Train train(arring_train_model[i], arriving_train_times_hours[i]); //��������
		trains.push_back(train); //���������������
	}

	/**
	 *ʹ�ü�-ֵ��key-value��ӳ��ʹÿ�������������ά�޼������Ӧ
	 *������train_idΪΨһ��ֵ��maintenance_level�����±�Ϊ��Ӧֵ��mapӳ��
	 */
	int train_maintenance_level_messages[arriving_train_numbers] = { 3,1,1,0,2,1,4 };
	/**
	 * ��ʼ��ʱ�����
	 */
	myTm first_arrived_time; //��һ����������ʱ��
	myTm last_arrived_time;//���һ����������ʱ��
	if (trains.size() > 0) {
		first_arrived_time = trains[0].arrive_time;
		last_arrived_time = trains[trains.size() - 1].arrive_time;
	}
	//��ǰʱ�����Ϊ��һ�������ĵ���ʱ��
	myTm now_time = first_arrived_time;//��ǰʱ��
	//����ʼʱ��Ϊ��һ����������ʱ��
	myTm start_time = last_arrived_time; //����ʼʱ��
	myTm step_time(0, STEP_TIME);//ʱ�����Ų���Ϊ1����
	//�ܼ�ʱ������
	myTm total_time;
	int cur = 0; //��ǰ�ȴ�����ά�޶��еĶ���
	while (true) {
		/**
		 *�жϵ�ǰʱ��������һ�������ĵ���ʱ��֮
		 *������ж���ά�޹����������ж�Ϊ��
		 *��ôά�޹���ȫ�����
		 *�˳�����
		 */
		if (!now_time.cmp(last_arrived_time))//��ǰʱ��������һ�����������ʱ��ʱ
		{
			/**
			  * ���ñ�־allFactoryEmpty bool����
			  * true:ȫ��ά�����
			  * false��δȫ��ά�����
			  * Ĭ��ΪΪtrue
			  */
			bool allFactoryEmpty = true;
			//����ȫ���������У����ֲ�Ϊ�յĹ��������������ʾά�޹���δ���
			for (int i = 0; i < numbers; i++) {
				if (!factories[i].empty()) {
					allFactoryEmpty = false;
					break;
				}
			}
			//���ȫ������Ϊ�գ�����ά������˳�����
			if (allFactoryEmpty) {
				//ʱ�����һ���ӣ�ȥ�����������ʱ��ƫ��
				total_time = total_time - step_time;
				now_time = now_time - step_time;
				break;
			}
		}
		/** ��ǰʱ�䵽�ﶯ����վʱ��
		  * �ö�������a����ȴ�����
		  * �ȴ���վ�����������1
		  */
		if (cur < trains.size()
			&& now_time.hour == trains[cur].arrive_time.hour
			&& now_time.minute == trains[cur].arrive_time.minute) {
			//��ǰ������Ӧ��ά�޼���
			int level = train_maintenance_level_messages[cur];
			//��ǰά�޼����ά�޵�һ����
			int process = maintenance_level[level][0];
			//��һά�޹���Ϊ�չ���������ά�޹���ȴ�������
			if (process != -1) {
				waitting_trains_id[process - 1].push(cur);
			}
			//�����α�����
			cur++;
		}

		//forѭ��numbers�ι���
		for (int i = 0; i < numbers; i++) {
			//��ǰ���򳵼�Ķ�ͷԪ��
			Factory f;
			if (!factories[i].empty()) {
				f = factories[i].top();
			}
			//��ǰ����ǿ��ҳ���ά�޳����Ľ���ʱ��С�ڵ�ǰʱ�䣬�򽫵�ǰ�������Ӳ�������һά�޵ȴ�����
			while (!factories[i].empty() && f.end_work_time.cmp(now_time)) {
				Factory factory = factories[i].top(); //��ȡ��ǰ����Ԫ��
				factories[i].pop(); //����			
				//��ǰ������Ӧ��ά�޼���
				int level = train_maintenance_level_messages[factory.train_id];
				//��ǰά�޼����ά����һ����
				int process = maintenance_level[level][i + 1];
				//��һά�޹���Ϊ��������ֵ��-1�����������һά�޹���ȴ�������
				if (process != -1) {
					waitting_trains_id[process - 1].push(factory.train_id);
				}
				free_factory_ids[i].push(factory.id);//��ӽ��뵱ǰ�������
				if (!factories[i].empty()) {
					f = factories[i].top();
				}
			}
			//��ǰ������г��䲻Ϊ�ղ��ҵ�ǰ����ȴ�������Ϊ��
			while (!free_factory_ids[i].empty() && !waitting_trains_id[i].empty()) {
				Factory factory;
				factory.id = free_factory_ids[i].front(); //��ǰ����id���Կ��г���
				free_factory_ids[i].pop();//���г������һ��
				factory.train_id = waitting_trains_id[i].front(); //�õ���ǰ�ȴ�����id
				waitting_trains_id[i].pop();//�ȴ���������һ��
				trains[factory.train_id].working_start_time[i] = now_time; //��ӵ�ǰ�������ʱ��				
				myTm working_time;//��ǰ����Ĺ���ʱ��
				//��ǰ����Ĺ���ʱ�����Ե�ǰ�������͹���ʱ��ɢ�б��ж�Ӧ��ֵ
				working_time.setTime(working_time_hash[trains[factory.train_id].model][i]);
				trains[factory.train_id].working_end_time[i] = now_time + working_time; //��ӵ�ǰ�������ʱ��
				factory.start_work(now_time, working_time); //���俪ʼ����				
				factories[i].push(factory); //������빤������
				trains[factory.train_id].factory_id[i] = factory.id; //��ӵ�ǰ���򳵼�				
			}
		}
		total_time = total_time + step_time; //������ʱ������һ������������ʱ��
		now_time = now_time + step_time; //��ǰʱ������һ������������ʱ��
	}
	cout << "��� ��� ����ʱ�� A������ A��ʼ A����   B������  B��ʼ  B����   C������ C��ʼ  C����   D������  D��ʼ  D����  E������ E��ʼ  E����" << endl;
	for (int i = 0; i < trains.size(); i++) {
		printf(" %02d", i);
		trains[i].print();
	}
	//total_time = now_time - start_time;
	//��ӡ��ʱ��Ϣ
	cout << "ά�����ж�����ʱ���ƣ�" << total_time.hour << "ʱ" << total_time.minute << "��" << endl;
	system("pause"); //����̨��ͣ
	return 0;
}