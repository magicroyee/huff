#include "hufftree.h"

int main()
{
	while (1)
	{
		int opt, flag = 0;			   // ÿ�ν���ѭ����Ҫ��ʼ��flagΪ0�����ں����ж��ļ����Ƿ����  opt�û�ѡ�����
		char ifname[256], ofname[256]; // ������������ļ���
	p:
		cout << "������ѡ����:\n 1: ѹ��\n 2: ��ѹ��\n 3: �˳�\n";
		cin >> opt;
		while (getchar() != '\n');
		if (opt == 3)
			break;
		else if (opt != 1 && opt != 2 && opt != 3)
		{
			cout << "�����������������" << endl;
			goto p;
		}
		else
		{
			cout << "�������ļ����ƣ�";
			cin.get(ifname, 256);
			while (getchar() != '\n');
			cout << "��������Ҫ������ļ�����: ";
			cin.get(ofname, 256);
			while (getchar() != '\n');
		}
		switch (opt)
		{
		case 1:

			flag = compress(ifname, ofname); // ѹ��������ֵ�����ж��Ƿ��ļ���������
			if (flag == -1)
				cout << "ԭ�ļ������ڣ�����\n"; // �����־Ϊ��-1���������ļ�������
			else
			{
				cout << "ѹ���С���\n";
				cout << "�������!\n"; // �������
			}
			break;
		case 2:
			flag = extract(ifname, ofname); // ��ѹ������ֵ�����ж��Ƿ��ļ���������
			if (flag == -1)
				cout << "ԭ�ļ������ڣ�����\n"; // �����־Ϊ��-1���������ļ�������
			else
			{
				cout << "��ѹ�С���\n";
				cout << "�������!\n"; // �������
			}
			break;
		}
	}
	return 0;
}