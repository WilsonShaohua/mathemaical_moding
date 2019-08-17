#pragma warning (disable:4996)

#include<bits/stdc++.h> //����ͷ�ļ�vs��֧��dev��ʹ��
using namespace std;
#define ARRIVE_TIME 15 //�г���վʱ��������λ�����ӣ� 
#define FACTORY_A_TIME 1*60 //����A����ʱ�� ����λ:���ӣ�
#define FACTORY_B_TIME 2*60 //����B����ʱ�� ����λ:���ӣ�
#define FACTORY_C_TIME 90 //����C����ʱ�䣨��λ�����ӣ� 
#define ALL_TIME 12*60 //������ʱ�䣨��λ�����ӣ�
#define START_TIME 0 //�г�����ʱ�� ����λ�����ӣ�

const int numbers = 3; //������

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
		printf("  %02d:%02d", hour%24, minute);
	}
};

//�г��ṹ�� 
struct _Train {
	int model; //�г��ͺ�
	myTm arrive_time; //�г�����ʱ�� 
	int factory_id[numbers]; //�����Ӧ����id
	myTm working_start_time[numbers]; //����ʼʱ��
	myTm working_end_time[numbers]; //�������ʱ��
	/**
	 *���캯������ʼ���г��ͺź͵�վʱ��͹���ʱ��
	 */
	_Train(int _model, myTm _arrive_time)
		:model(_model), arrive_time(_arrive_time) {}
	/**
	 *���캯��
	 *��ʼ����վʱ��
	 *�ͺ�Ĭ��Ϊ0
	 */
	_Train(myTm _arrive_time) :arrive_time(_arrive_time) {
		model = 0;
	};


	//�г�ά��ʱ����ӡ����
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
	int train_id; //��ǰ����ά���г�id��
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
	int factory_number[numbers] = { 3,8,5 };//���������
	queue<int> free_factory_ids[numbers]; //���г���id��
	//ʹ�����ȶ���
	priority_queue<Factory> factories[3]; //��������ĳ���
	//��ʼ����������ĳ�����������ǰ���г���ȫ�����ڿ���״̬
	for (int i = 0; i < numbers; i++) {
		//���쵱ǰ���򳵼��id
		for (int j = 1; j <= factory_number[i]; j++) {
			free_factory_ids[i].push(j);
		}
	}

	/**
	 *��ʼ��trains����
	 *��������г������г�������
	 */
	vector<_Train> trains;//�г�����ʱ�̱����
	queue<int> waitting_trains_id[numbers + 1]; //�ȴ�������һ������г���id
	time_t now = time(0);//��ȡϵͳʱ�䣬��1900/0/1 0:0:0��ʼ������
	tm* local_time = localtime(&now);//��ϵͳʱ��ת��Ϊ����ʱ��
	myTm now_time(local_time->tm_hour, local_time->tm_min);//������ʱ���ȡСʱ�ͷ���
	myTm end_time(0, ALL_TIME);//����ֹͣ����ʱ��
	end_time = end_time + now_time;
	myTm step_time(0, ARRIVE_TIME);//������֮�䵽����ʱ��
	myTm start_time(0, START_TIME);//��������ʱ�����ʼʱ��
	start_time = now_time; //��ʼʱ����ڵ�ǰʱ��
	//��ʼ�����乤��ʱ��
	myTm factory_working_time[numbers];
	factory_working_time[0].minute = FACTORY_A_TIME % 60;
	factory_working_time[0].hour = FACTORY_A_TIME / 60;
	factory_working_time[1].minute = FACTORY_B_TIME % 60;
	factory_working_time[1].hour = FACTORY_B_TIME / 60;
	factory_working_time[2].minute = FACTORY_C_TIME % 60;
	factory_working_time[2].hour = FACTORY_C_TIME / 60;


	myTm total_time(0, 0);//�ܼƺ�ʱ
	myTm _now = now_time; //��ǰʱ��
	

	/**
	 *��ǰ���в��յ�����¼�������
	 */
	while (true) {

		//��ǰʱ��С���г�ֹͣ����ʱ���������ӽ����г������С������ó���id����ȴ�����1��
		if (_now.cmp(end_time)) {
			_Train train(_now);//��ǰʱ��С�ڳ���ֹͣ����ʱ�䣬�����µ��г�����ά��i����
			trains.push_back(train);
			waitting_trains_id[0].push(trains.size() - 1); //��ǰ�����г�id����ȴ����빤��0����
		}
		else {//��ǰʱ�䳬���г�����ʱ���ж����г����Ƿ�ȫ��Ϊ��
			bool allFactoryEmpty = true;//���ñ�־flagΪtrue��ȫ��ά����ϣ�false��δȫ��ά�����
			for (int i = 0; i < numbers; i++) {
				if (!factories[i].empty()) {
					allFactoryEmpty = false;
					break;
				}
			}
			//���ȫ������Ϊ�գ�����ά������˳�����
			if (allFactoryEmpty) {
				break;
			}
		}

		//forѭ��numbers�ι���
		for (int i = 0; i < numbers; i++) {
			//��ǰ���򳵼�Ķ�ͷԪ��
			Factory f;
			if (!factories[i].empty()) {
				f = factories[i].top();
			}
			//��ǰ����ǿ��ҳ���ά�޳����Ľ���ʱ��С�ڵ�ǰʱ�䣬�򽫵�ǰ�������Ӳ�������һά�޵ȴ�����
			while (!factories[i].empty() && f.end_work_time.cmp(_now)) {

				Factory factory = factories[i].top(); //��ȡ��ǰ����Ԫ��
				factories[i].pop(); //����				
				waitting_trains_id[i + 1].push(factory.train_id); //������һ����ȴ�����
				free_factory_ids[i].push(factory.id);//��ӽ��뵱ǰ�������
				if (!factories[i].empty()) {
					f = factories[i].top();
				}
			}
			//��ǰ������г��䲻Ϊ�ղ��ҵ�ǰ����ȴ��г���Ϊ��
			while (!free_factory_ids[i].empty() && !waitting_trains_id[i].empty()) {
				Factory factory;
				factory.id = free_factory_ids[i].front(); //��ǰ����id���Կ��г���
				free_factory_ids[i].pop();//���г������һ��
				factory.train_id = waitting_trains_id[i].front(); //�õ���ǰ�ȴ��г�id
				waitting_trains_id[i].pop();//�ȴ��г�����һ��
				trains[factory.train_id].working_start_time[i] = _now; //��ӵ�ǰ�������ʱ��
				trains[factory.train_id].working_end_time[i] = _now + factory_working_time[i]; //��ӵ�ǰ�������ʱ��
				factory.start_work(_now, factory_working_time[i]); //���俪ʼ����				
				factories[i].push(factory); //������빤������
				trains[factory.train_id].factory_id[i] = factory.id; //��ӵ�ǰ���򳵼�				
			}
		}
		//total_time = total_time + step_time; //������ʱ������һ���г�������ʱ��
		_now = _now + step_time; //��ǰʱ������һ���г�������ʱ��
	}
	cout << "��� �ͺ� ����ʱ�� A������ A��ʼ A���� B������ B��ʼ B���� C������ C��ʼ C����" << endl;
	for (int i = 0; i < trains.size(); i++) {
		printf(" %02d", i);
		trains[i].print();
	}
	total_time = _now - start_time; //��ʱ��Ϊ���һ���г��������ʱ�䣨��ǰʱ�䣩ʱ���ȥ�г�����ʱ��
	//��ӡ��ʱ��Ϣ
	cout << "ά�������г���ʱ���ƣ�" << total_time.hour << "ʱ" << total_time.minute << "��" << endl;
	system("pause"); //����̨վ��
	return 0;
}