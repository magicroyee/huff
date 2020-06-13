#include "hufftree.h"
#include <graphics.h>
#include <conio.h>

void select(HufNode *huf_tree, unsigned int n, int *s1, int *s2)
{
	// 找最小
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for (i = 0; i < n; ++i) //选择两个其双亲域为0且权值最小的结点，并返回它们在HT中的序号s1和s2
		if (huf_tree[i].parent == -1 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
	huf_tree[*s1].parent = 1; // 标记此结点已被选中

	// 找次小
	min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huf_tree[i].parent == -1 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
}

void CreateTree(HufNode *huf_tree, unsigned int char_kinds, unsigned int node_num)
{
	unsigned int i;
	int s1, s2;
	for (i = char_kinds; i < node_num; ++i)
	{
		select(huf_tree, i, &s1, &s2); // 选择最小的两个结点
		huf_tree[s1].parent = huf_tree[s2].parent = i;
		huf_tree[i].lchild = s1;
		huf_tree[i].rchild = s2;
		huf_tree[i].weight = huf_tree[s1].weight + huf_tree[s2].weight;
	}
}

void HufCode(HufNode *huf_tree, unsigned char_kinds)
{
	unsigned int i;
	int cur, next, index;
	char *code_tmp = new char[256];
	code_tmp[256 - 1] = '\0';

	for (i = 0; i < char_kinds; ++i)
	{
		index = 256 - 1; // 编码临时空间索引初始化

		// 从叶子向根反向遍历求编码
		for (cur = i, next = huf_tree[i].parent; next != -1; cur = next, next = huf_tree[next].parent)
			if (huf_tree[next].lchild == cur)
				code_tmp[--index] = '0'; // 左‘0’
			else
				code_tmp[--index] = '1'; // 右‘1’
		huf_tree[i].code = new char[256 - index];
		strcpy(huf_tree[i].code, &code_tmp[index]); // 正向保存编码到树结点相应域中
	}
	delete[] code_tmp; // 释放编码临时空间
}

int compress(char *ifname, char *ofname)
{
	unsigned int i, j;
	unsigned int char_kinds;	// 字符种类
	unsigned char char_temp;	// 暂存8bits字符
	unsigned long file_len = 0; //读入的txt文件中的字符的总长度
	FILE *infile, *outfile;
	TmpNode node_temp; //TmpNode用它来定义一个临时节点
	unsigned int node_num;
	HufNode *huf_tree;
	char code_buf[256] = "\0"; // 待存编码缓冲区
	unsigned int code_len;
	/*
	** 动态分配256个结点，暂存字符频度，
	** 统计并拷贝到树结点后立即释放
	*/
	TmpNode *tmp_nodes = new TmpNode[256];
	// 初始化暂存结点。一个unsigned char类型是8位，即1个字节，所有的字符都可以用这8位来表示，一共256种字符种类，所以下面用i对应相应的字符种类，为了后面的统计字符种类方便
	for (i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].uch = (unsigned char)i; // 数组的256个下标与256种字符对应，为了后面统计字符种类更方便
	}

	infile = fopen(ifname, "rb"); //infile是个文件指针(FILE*).FilefullName（ifname）是个文件名,类型是字符串(char*).rb表示是看成二进制打开并读该文件，r表示read，读文件
	// 判断输入文件是否存在
	if (infile == NULL) //如果打开失败，则infile返回空指针
		return -1;
	fread((char *)&char_temp, sizeof(unsigned char), 1, infile); //从infile中读入1个sizeof(unsigned char)这样大小的字节
	// 遍历文件，获取字符频度
	while (!feof(infile))
	{
		++tmp_nodes[char_temp].weight;								 // 统计下标对应字符的权重，利用数组的随机访问快速统计字符频度，因为tmp_nodes[i].uch = (unsigned char)i;数组的256个下标与256种字符对应，所以可以直接用下标对应上字符种类
		++file_len;													 //每读入一个字符，文件字符总长度加1，得出字符总长度
		fread((char *)&char_temp, sizeof(unsigned char), 1, infile); //从infile中读入1个sizeof(unsigned char)这样大小的字节
	}
	fclose(infile); //读完，关闭文件

	// 排序，主要目的是将频度为零的放最后，进行下一步的剔除。
	for (i = 0; i < 256 - 1; ++i)
		for (j = i + 1; j < 256; ++j)
			if (tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				node_temp = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = node_temp;
			}
	// 统计实际的字符种类（出现次数不为0）
	for (i = 0; i < 256; ++i)
		if (tmp_nodes[i].weight == 0) //一直循环到频度为0时，则i就表示字符的种类数，因为上一步已经将频度为0的放到了后面
			break;
	char_kinds = i; //i就表示了字符种类数
	if (char_kinds < 1)
		return -1; //输入有误

	if (char_kinds == 1) //如果只有一个字符
	{
		outfile = fopen(ofname, "wb");											 // 打开压缩后将生成的文件
		fwrite((char *)&char_kinds, sizeof(unsigned int), 1, outfile);			 // 写入字符种类
		fwrite((char *)&tmp_nodes[0].uch, sizeof(unsigned char), 1, outfile);	 // 写入唯一的字符
		fwrite((char *)&tmp_nodes[0].weight, sizeof(unsigned long), 1, outfile); // 写入字符频度，也就是文件长度
		delete[] tmp_nodes;
		fclose(outfile);
	}
	else if (char_kinds > 1) //如果种类不只一种字符种类
	{
		node_num = 2 * char_kinds - 1; // 根据字符种类数，计算建立哈夫曼树所需结点数，相当于m=2*n-1
		huf_tree = new HufNode[node_num];
		// 初始化前char_kinds个结点
		for (i = 0; i < char_kinds; ++i)
		{
			// 将暂存结点的字符和频度拷贝到树结点
			huf_tree[i].uch = tmp_nodes[i].uch;
			huf_tree[i].weight = tmp_nodes[i].weight;
			huf_tree[i].parent = -1;
			huf_tree[i].lchild = huf_tree[i].rchild = -1;
		}
		//		free(tmp_nodes); // 释放字符频度统计的暂存区
		delete[] tmp_nodes;
		// 初始化后node_num-char_kins个结点
		for (; i < node_num; ++i)
			huf_tree[i].parent = -1;

		CreateTree(huf_tree, char_kinds, node_num); // 创建哈夫曼树
		drawing(huf_tree, char_kinds, node_num);

		HufCode(huf_tree, char_kinds); // 生成哈夫曼编码

		// 写入字符和相应权重，供解压时重建哈夫曼树
		outfile = fopen(ofname, "wb"); // 打开压缩后将生成的文件
		fwrite((char *)&char_kinds, sizeof(unsigned int), 1, outfile);
		for (i = 0; i < char_kinds; ++i) //下面两句是为了按顺序写入待生成文件
		{
			fwrite((char *)&huf_tree[i].uch, sizeof(unsigned char), 1, outfile);	// 写入字符（已排序，读出后顺序不变）
			fwrite((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, outfile); // 写入字符对应权重
		}

		// 紧接着字符和权重信息后面写入文件长度和字符编码
		fwrite((char *)&file_len, sizeof(unsigned long), 1, outfile); // 写入文件长度
		infile = fopen(ifname, "rb");								  // 以二进制形式打开待压缩的文件
		fread((char *)&char_temp, sizeof(unsigned char), 1, infile);  // 每次读取8bits
		while (!feof(infile))										  //当没有读到文件末尾的时候
		{
			// 匹配字符对应编码
			for (i = 0; i < char_kinds; ++i)
				if (char_temp == huf_tree[i].uch)
					strcat(code_buf, huf_tree[i].code);
			while (strlen(code_buf) >= 8) //因为是以8bit进行读取的
			{
				char_temp = '\0'; // 清空字符暂存空间，改为暂存字符对应编码
				for (i = 0; i < 8; ++i)
				{
					char_temp <<= 1; // 左移一位，为下一个bit腾出位置，这三句就是a=a*2+(str[i]-'0').(str[i]-'0')是将一个char类型转换成了整型。a=a*2参考：321=（（3*10+2）*10+1），其实就是大整数进行加法的时候，将string的每一个字符换成整型
					if (code_buf[i] == '1')
						char_temp |= 1; // 当编码为"1"，通过或操作符将其添加到字节的最低位。   |= 1表示或1运算。相当于上面一行中321后面的+2和+1的过程
				}
				fwrite((char *)&char_temp, sizeof(unsigned char), 1, outfile); // 将字节对应编码存入文件
				strcpy(code_buf, code_buf + 8);								   // 编码缓存去除已处理的前八位
			}
			fread((char *)&char_temp, sizeof(unsigned char), 1, infile); // 每次读取8bits
		}
		// 处理最后不足8bits编码
		code_len = strlen(code_buf);
		if (code_len > 0) //这里加入的判断是为了防止上一步进行每8位读取时，正好长度是8的倍数。
		{
			char_temp = '\0';
			for (i = 0; i < code_len; ++i)
			{
				char_temp <<= 1;
				if (code_buf[i] == '1')
					char_temp |= 1;
			}
			char_temp <<= 8 - code_len;									   // 将编码字段从尾部移到字节的高位，加0补齐
			fwrite((char *)&char_temp, sizeof(unsigned char), 1, outfile); // 存入最后一个字节
		}
		// 关闭文件
		fclose(infile);
		fclose(outfile);
		// 释放内存
		for (i = 0; i < char_kinds; ++i)
			delete huf_tree[i].code;
		delete[] huf_tree;
	}
	return 0;
}

int extract(char *ifname, char *ofname)
{
	unsigned int i;
	unsigned long file_len;
	unsigned long writen_len = 0; // 控制文件写入长度
	FILE *infile, *outfile;
	unsigned int char_kinds; // 存储字符种类
	unsigned int node_num;
	HufNode *huf_tree;
	unsigned char code_temp; // 暂存8bits编码
	unsigned int root;		 // 保存根节点索引，供匹配编码使用

	infile = fopen(ifname, "rb"); // 以二进制方式打开压缩文件
	// 判断输入文件是否存在
	if (infile == NULL)
		return -1;

	// 读取压缩文件前端的字符及对应编码，用于重建哈夫曼树
	fread((char *)&char_kinds, sizeof(unsigned int), 1, infile); // 读取字符种类数
	if (char_kinds == 1)										 //如果只有一种字符
	{
		fread((char *)&code_temp, sizeof(unsigned char), 1, infile); // 读取唯一的字符
		fread((char *)&file_len, sizeof(unsigned long), 1, infile);	 // 读取文件长度
		outfile = fopen(ofname, "wb");								 // 打开压缩后将生成的文件
		while (file_len--)
			fwrite((char *)&code_temp, sizeof(unsigned char), 1, outfile);
		fclose(infile);
		fclose(outfile);
	}
	else if (char_kinds > 1)
	{
		node_num = 2 * char_kinds - 1; // 根据字符种类数，计算建立哈夫曼树所需结点数
		huf_tree = new HufNode[node_num];
		// 读取字符及对应权重，存入哈夫曼树节点
		for (i = 0; i < char_kinds; ++i)
		{
			fread((char *)&huf_tree[i].uch, sizeof(unsigned char), 1, infile);	  // 读入字符
			fread((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, infile); // 读入字符对应权重
			huf_tree[i].parent = -1;
		}
		// 初始化后node_num-char_kins个结点的parent
		for (; i < node_num; ++i)
			huf_tree[i].parent = -1;

		CreateTree(huf_tree, char_kinds, node_num); // 重建哈夫曼树（与压缩时的一致）

		// 读完字符和权重信息，紧接着读取文件长度和编码，进行解码
		fread((char *)&file_len, sizeof(unsigned long), 1, infile); // 读入文件长度
		outfile = fopen(ofname, "wb");								// 打开压缩后将生成的文件
		root = node_num - 1;
		while (1)
		{
			fread((char *)&code_temp, sizeof(unsigned char), 1, infile); // 读取一个字符长度的编码

			// 处理读取的一个字符长度的编码（通常为8位）
			for (i = 0; i < 8; ++i)
			{
				// 由根向下直至叶节点正向匹配编码对应字符
				if (code_temp & 128)
					root = huf_tree[root].rchild;
				else
					root = huf_tree[root].lchild;

				if (root < char_kinds)
				{
					fwrite((char *)&huf_tree[root].uch, sizeof(unsigned char), 1, outfile);
					++writen_len;
					if (writen_len == file_len)
						break;			 // 控制文件长度，跳出内层循环
					root = node_num - 1; // 复位为根索引，匹配下一个字符
				}
				code_temp <<= 1; // 将编码缓存的下一位移到最高位，供匹配
			}
			if (writen_len == file_len)
				break; // 控制文件长度，跳出外层循环
		}

		// 关闭文件
		fclose(infile);
		fclose(outfile);
		// 释放内存
		delete[] huf_tree;
	}
	return 0;
}

void drawing(HufNode *huf_tree, int char_kinds, int node_num)
{
	int gapx = (WIDTH - 100) / char_kinds;
	int gapy = 40;

	int root = 0;
	while (huf_tree[root].parent != -1)
	{
		root = huf_tree[root].parent;
	}
	treeNum(huf_tree, root, 1);
	for (int i = 0; i < node_num; i++)
	{
		huf_tree[i].idx = huf_tree[i].idx /
						  pow(2, (int)(log10(huf_tree[i].idx) / log10(2)));
	}

	int no = 1;
	for (int i = 0; i < char_kinds; i++)
	{
		no = 1;
		for (int j = 0; j < char_kinds; j++)
		{
			if (huf_tree[j].idx < huf_tree[i].idx)
			{
				no++;
			}
		}
		huf_tree[i].p.x = gapx * no;
	}
	int lc, rc;
	for (int i = char_kinds; i < node_num; i++)
	{
		lc = huf_tree[i].lchild;
		rc = huf_tree[i].rchild;
		huf_tree[i].p.x = (huf_tree[lc].p.x + huf_tree[rc].p.x) / 2;
	}
	for (int i = 0; i < node_num; i++)
	{
		huf_tree[i].p.y = gapy * int(log10(huf_tree[i].idy) / log10(2)) + 50;
	}

	initgraph(WIDTH, HEIGHT); //初始化绘图屏幕
	setcolor(WHITE);
	setbkcolor(WHITE);
	setfillstyle(WHITE);
	cleardevice();
	setcolor(BLACK);

	draw(huf_tree, root);

	getchar();
	closegraph();
}

void treeNum(HufNode *huf_tree, int root, int rootnum)
{
	int lc = huf_tree[root].lchild;
	int rc = huf_tree[root].rchild;
	huf_tree[root].idx = rootnum;
	huf_tree[root].idy = rootnum;
	if (lc != -1)
	{
		treeNum(huf_tree, lc, rootnum * 2);
	}
	if (rc != -1)
	{
		treeNum(huf_tree, rc, rootnum * 2 + 1);
	}
}

void draw(HufNode *huf_tree, int root)
{
	int lc = huf_tree[root].lchild;
	int rc = huf_tree[root].rchild;
	int x = huf_tree[root].p.x;
	int y = huf_tree[root].p.y;
	circle(x, y, 2);
	WCHAR arr[10];
	wsprintf(arr, L"%d", huf_tree[root].weight);
	setcolor(BLUE);
	outtextxy(x + 3, y - 8, arr);
	setcolor(BLACK);
	if (lc == -1)
	{
		outtextxy(x, y + 5, huf_tree[root].uch);
	}
	else
	{
		line(x, y, (int)huf_tree[lc].p.x, (int)huf_tree[lc].p.y);
		line(x, y, (int)huf_tree[rc].p.x, (int)huf_tree[rc].p.y);
		setcolor(RED);
		outtextxy((x + (int)huf_tree[lc].p.x) / 2, (y + (int)huf_tree[lc].p.y) / 2, '0');
		outtextxy((x + (int)huf_tree[rc].p.x) / 2, (y + (int)huf_tree[rc].p.y) / 2, '1');
		setcolor(BLACK);
		draw(huf_tree, lc);
		draw(huf_tree, rc);
	}
}