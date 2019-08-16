#pragma warning (disable:4996)

#include<bits/stdc++.h> //����ͷ�ļ�vs��֧��dev��ʹ��
using namespace std;
#define ARRIVE_TIME 15 //�г���վʱ��������λ�����ӣ� 
#define FACTORY_A_TIME 1*60 //����A����ʱ�� ����λ���ӣ�
#define FACTORY_B_TIME 2*60 //����B����ʱ�� ����λ�����У�
#define FACTORY_C_TIME 90 //����C����ʱ�䣨��λ���ӣ� 
#define ALL_TIME 12*60 //������ʱ�䣨���ӣ�
#define START_TIME 0 //�г�����ʱ�� ����λ���ӣ�

const int numbers = 3; //������

/**
 *�Զ���ʱ��ṹ��
 *����Сʱ�ͷ�����������
 */
struct _Tm {
	int hour; //Сʱ
	int minute;//����
	/**
	 *��ʼ������
	 *����˵��
	 *_hour������hour
	 *_minute:����minute
	 */
	_Tm(int _hour, int _minute) :hour(_hour), minute(_minute) {
		hour = hour + minute / 60;
		minute = minute % 60;
	}

	/**
	 *���캯��
	 */
	_Tm() {}


	/*
	 *�����������<=
	 *��ʱ����Ľ�С
	 */
	bool operator<= (_Tm &time) {
		if (hour <= time.hour) {
			return minute <= time.hour;
		}
		return hour < time.hour;
	}

	/*
	 *�����������<
	 *��ʱ����Ľ�С
	 */
	bool operator< (const _Tm time) {
		if (hour == time.hour) {
			return minute < time.minute;
		}
		return hour < time.hour;
	}

	/*
	 *�����������==
	 *hour �� minute���Ա�����ȫһ��
	 */
	bool operator== (const _Tm &time) {
		return (hour == time.hour) && (minute == time.minute);
	}
	/*
	 *�����������+
	 *��ʱ����Ľ�С
	 */
	_Tm operator+(const _Tm &time) {
		_Tm t(0, 0);
		t.minute = minute + time.minute;
		t.hour = hour + time.hour +t.minute/ 60;
		t.minute = t.minute % 60;
		return t;
	}

	/*
	 *�����������-
	 *��ʱ����Ľ�С
	 */
	_Tm operator-(const _Tm &time) {
		_Tm t(0, 0);
		t.minute = minute - time.minute;
		t.hour = hour - time.hour;
		if (t.minute < 0) {
			t.minute = t.minute + 60;
			t.hour = t.hour - 1;
		}		
		return t;
	}

	void print() {
		cout << " " << hour << ":" << minute;
	}
};

//�г��ṹ�� 
struct _Train {
	int model; //�г��ͺ�
	_Tm arrive_time; //�г�����ʱ�� 
	int factory_id[numbers]; //�����Ӧ����id
	_Tm working_start_time[numbers]; //����ʼʱ��
	_Tm working_end_time[numbers]; //�������ʱ��
	/**
	 *���캯������ʼ���г��ͺź͵�վʱ��͹���ʱ��
	 */
	_Train(int _model, _Tm _arrive_time) 
		:model(_model), arrive_time(_arrive_time){}
	/**
	 *���캯��
	 *��ʼ����վʱ��
	 *�ͺ�Ĭ��Ϊ0
	 */
	_Train( _Tm _arrive_time) :arrive_time(_arrive_time) {
		model = 0;
	};

	
	/**
	 *���ص��������
	 */
	bool operator= (const _Train train) {
		model = train.model;
		arrive_time = train.arrive_time;
		for (int i = 0; i < numbers; i++) {
			working_start_time[i] = train.working_start_time[i];
			working_end_time[i] = train.working_end_time[i];
			factory_id[i] = train.factory_id[i];
		}
	}
	//�г�ά��ʱ����ӡ����
	void print() {
		cout << " CRH" << model;
		arrive_time.print();
		for (int i = 0; i < numbers; i++) {
			cout << factory_id[i];  working_start_time[i].print(); working_end_time[i].print();
		}		
		cout << endl;	
	}
	//��ȡ��ǰ��cur������Ĺ���ʱ�䣺working_end_time - working_start_time
	_Tm get_working_time(int cur) {
		return working_end_time[cur] - working_start_time[cur];
	}
};

//����ṹ�� 
struct Factory {
	int state;//����״̬��1Ϊ�����У�0Ϊ����
	int train_id; //��ǰ����ά���г�id��
	_Tm end_work_time;//����������ʱ��
	int id;//����id�� 
	/**
	 *���캯��
	 */
	Factory(int _id) :id(_id) {
		state = 0;
	}
	Factory() {
		state = 0;
	}
	/**
	 *��ȡ��ǰ����״̬
	 *���� 1 ״̬Ϊ��������
	 *���� 0 ״̬Ϊ������
	 */
	int getState(_Tm now_time) {
		if (state==1 && end_work_time <= now_time) {
			state = 0;
		}
		return state;
	}
	//��ʼ����
	void start_work(_Tm now_time,_Tm working_time) {
		end_work_time = end_work_time + working_time;
		state = 1;
	}
	/**
	 *����С�������
	 *�Ƚϸü����ļ乤���Ľ���ʱ������
	 */
	bool operator< (const Factory& factory) {
		if(factory.state==1 && state==1)
			return end_work_time < factory.end_work_time;
		return state < factory.state;
	}
};

int main() {
	int factory_number[numbers] = { 3,8,5 };//����������
	queue<int> free_factory_ids[numbers]; //���й���id��
	//ʹ�����ȶ���
	priority_queue<Factory> factories[3]; //��������Ĺ���
	//��ʼ����������Ĺ�����������ǰ���й���ȫ�����ڿ���״̬
	for (int i = 0; i < numbers; i++) {
		//���쵱ǰ���򹤳���id
		for (int j = 1; j <= factory_number[i]; j++) {
			free_factory_ids[i].push(j);
		}
	}

	/**
	 *��ʼ��trains����
	 *��������г������г�������
	 */
	vector<_Train> trains;//�г�����ʱ�̱����
	queue<int> waitting_trains_id[numbers+1]; //�ȴ�������һ������г���id
	time_t now = time(0);//��ȡϵͳʱ�䣬��1900/0/1 0:0:0��ʼ������
	tm* local_time = localtime(&now);//��ϵͳʱ��ת��Ϊ����ʱ��
	_Tm now_time(local_time->tm_hour, local_time->tm_min);//������ʱ���ȡСʱ�ͷ���
	_Tm end_time(0, ALL_TIME);//����ֹͣ����ʱ��
	end_time = end_time + now_time;
	_Tm step_time(0, ARRIVE_TIME);//������֮�䵽����ʱ��
	_Tm start_time(0, START_TIME);//��������ʱ�����ʼʱ��

	//��ʼ����������ʱ��
	_Tm factory_working_time[numbers];
	factory_working_time[0].minute = FACTORY_A_TIME % 60;
	factory_working_time[0].hour = FACTORY_A_TIME / 60;
	factory_working_time[1].minute = FACTORY_B_TIME % 60;
	factory_working_time[1].hour = FACTORY_B_TIME / 60;
	factory_working_time[2].minute = FACTORY_C_TIME % 60;
	factory_working_time[2].hour = FACTORY_C_TIME / 60;


	_Tm total_time(0, 0);//�ܼƺ�ʱ
	_Tm _now = now_time; //��ǰʱ��
	
	/**
	 *��ǰ���в��յ�����¼�������
	 */
	while (true) {
		
		//��ǰʱ��С���г�ֹͣ����ʱ���������ӽ����г������С������ó���id����ȴ�����1��
		if (_now < end_time) {
			_Train train(_now);//��ǰʱ��С�ڳ���ֹͣ����ʱ�䣬�����µ��г�����ά��i����
			trains.push_back(train);
			waitting_trains_id[0].push(trains.size()-1); //��ǰ�����г�id����ȴ����빤��0����
		}
		else {//��ǰʱ�䳬���г�����ʱ���ж����й����Ƿ�ȫ��Ϊ��
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
			//��ǰ���򹤳��Ķ�ͷԪ��
			Factory f;
			if(!factories[i].empty()){
				f = factories[i].top();
			}
			//��ǰ�����ǿ��ҹ���ά�޳����Ľ���ʱ��С�ڵ�ǰʱ�䣬�򽫵�ǰ�������Ӳ�������һά�޵ȴ�����
			while (!factories[i].empty() && f.end_work_time <= _now) {
				Factory factory = factories[i].top(); //��ȡ��ǰ����Ԫ��
				factories[i].pop(); //����				
				waitting_trains_id[i + 1].push(factory.train_id); //������һ����ȴ�����
				free_factory_ids[i].push(factory.id);//��ӽ��뵱ǰ��������
				if (!factories[i].empty()) {
					f = factories[i].top();
				}
			}
			//��ǰ������й�����Ϊ�ղ��ҵ�ǰ����ȴ��г���Ϊ��
			while (!free_factory_ids[i].empty() && !waitting_trains_id[i].empty()) {
				Factory factory;
				factory.id = free_factory_ids[i].front(); //��ǰ����id���Կ��й���
				free_factory_ids[i].pop();//���й�������һ��
				factory.train_id = waitting_trains_id[i].front(); //�õ���ǰ�ȴ��г�id
				waitting_trains_id[i].pop();//�ȴ��г�����һ��
				trains[i].working_start_time[i] = _now; //��ӵ�ǰ�������ʱ��
				trains[i].working_end_time[i] = _now + factory_working_time[i]; //��ӵ�ǰ�������ʱ��
				factory.start_work(_now, factory_working_time[i]); //������ʼ����
				factories[i].push(factory); //�������빤������
				trains[factory.train_id].factory_id[i] = factory.id; //��ӵ�ǰ���򹤳�				
			}
		}
		_now = _now + step_time; //ʱ��ǰ��һ��������
	}
	cout << " �ͺ� ����ʱ�� A��ʼ A���� B��ʼ B���� C��ʼ C����" << endl;
	for (auto i = 0; i < trains.size(); i++) {
		trains[i].print();
	}
	total_time = _now - start_time; //����ʱ=��ǰʱ��-��ʼʱ��
	//��ӡ��ʱ��Ϣ
	cout<<"ά�������г���ʱ���ƣ�" << total_time.hour << "ʱ" << total_time.minute << "��" << endl;
	system("pause"); //����̨վ��
	return 0;
}