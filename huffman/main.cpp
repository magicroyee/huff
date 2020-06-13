#include "hufftree.h"

int main()
{
	while (1)
	{
		int opt, flag = 0;			   // 每次进入循环都要初始化flag为0，用于后面判断文件名是否存在  opt用户选择操作
		char ifname[256], ofname[256]; // 保存输入输出文件名
	p:
		cout << "请输入选择项:\n 1: 压缩\n 2: 解压缩\n 3: 退出\n";
		cin >> opt;
		while (getchar() != '\n');
		if (opt == 3)
			break;
		else if (opt != 1 && opt != 2 && opt != 3)
		{
			cout << "输入错误，请重新输入" << endl;
			goto p;
		}
		else
		{
			cout << "请输入文件名称：";
			cin.get(ifname, 256);
			while (getchar() != '\n');
			cout << "请输入需要输出的文件名称: ";
			cin.get(ofname, 256);
			while (getchar() != '\n');
		}
		switch (opt)
		{
		case 1:

			flag = compress(ifname, ofname); // 压缩，返回值用于判断是否文件名不存在
			if (flag == -1)
				cout << "原文件不存在！！！\n"; // 如果标志为‘-1’则输入文件不存在
			else
			{
				cout << "压缩中……\n";
				cout << "操作完成!\n"; // 操作完成
			}
			break;
		case 2:
			flag = extract(ifname, ofname); // 解压，返回值用于判断是否文件名不存在
			if (flag == -1)
				cout << "原文件不存在！！！\n"; // 如果标志为‘-1’则输入文件不存在
			else
			{
				cout << "解压中……\n";
				cout << "操作完成!\n"; // 操作完成
			}
			break;
		}
	}
	return 0;
}