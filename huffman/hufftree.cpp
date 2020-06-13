#include "hufftree.h"

void select(HufNode* huf_tree, unsigned int n, int* s1, int* s2)
{
	// ����С
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for (i = 0; i < n; ++i) //ѡ��������˫����Ϊ0��Ȩֵ��С�Ľ�㣬������������HT�е����s1��s2
		if (huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
	huf_tree[*s1].parent = 1; // ��Ǵ˽���ѱ�ѡ��

	// �Ҵ�С
	min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
}

void CreateTree(HufNode* huf_tree, unsigned int char_kinds, unsigned int node_num)
{
	unsigned int i;
	int s1, s2;
	for (i = char_kinds; i < node_num; ++i)
	{
		select(huf_tree, i, &s1, &s2); // ѡ����С���������
		huf_tree[s1].parent = huf_tree[s2].parent = i;
		huf_tree[i].lchild = s1;
		huf_tree[i].rchild = s2;
		huf_tree[i].weight = huf_tree[s1].weight + huf_tree[s2].weight;
	}
}

void HufCode(HufNode* huf_tree, unsigned char_kinds)
{
	unsigned int i;
	int cur, next, index;
	char* code_tmp = new char[256];//(char*)malloc(256 * sizeof(char)); // �ݴ���룬���256��Ҷ�ӣ����볤�Ȳ�����255
	code_tmp[256 - 1] = '\0';

	for (i = 0; i < char_kinds; ++i)
	{
		index = 256 - 1; // ������ʱ�ռ�������ʼ��

		// ��Ҷ�����������������
		for (cur = i, next = huf_tree[i].parent; next != 0; cur = next, next = huf_tree[next].parent)
			if (huf_tree[next].lchild == cur)
				code_tmp[--index] = '0'; // ��0��
			else
				code_tmp[--index] = '1';								 // �ҡ�1��
		huf_tree[i].code = new char[256 - index];//(char*)malloc((256 - index) * sizeof(char)); // Ϊ��i���ַ����붯̬����洢�ռ�
		strcpy(huf_tree[i].code, &code_tmp[index]);						 // ���򱣴���뵽�������Ӧ����
	}
	delete[]code_tmp;//free(code_tmp); // �ͷű�����ʱ�ռ�
}

int compress(char* ifname, char* ofname)
{
	unsigned int i, j;
	unsigned int char_kinds;	// �ַ�����
	unsigned char char_temp;	// �ݴ�8bits�ַ�
	unsigned long file_len = 0; //�����txt�ļ��е��ַ����ܳ���
	FILE* infile, * outfile;
	//����file����ͣ�FILE* ָ����Ϊ�ļ���������ļ����ʵ�Ψһ��ʶ������fopen����������fopen���ļ��ɹ����򷵻�һ����Ч��FILE*ָ�룬���򷵻ؿ�ָ��NULL��������Ͳμ�http://blog.csdn.net/jiahehao/article/details/1862867
	TmpNode node_temp; //TmpNode����������һ����ʱ�ڵ�
	unsigned int node_num;
	HufNode * huf_tree;
	char code_buf[256] = "\0"; // ������뻺����
	unsigned int code_len;
	/*
	** ��̬����256����㣬�ݴ��ַ�Ƶ�ȣ�
	** ͳ�Ʋ������������������ͷ�
	*/
	TmpNode* tmp_nodes = new TmpNode[256];//(TmpNode*)malloc(256 * sizeof(TmpNode)); //malloc ��ϵͳ�������ָ��size���ֽڵ��ڴ�ռ䡣���������� void* ���͡�

	// ��ʼ���ݴ��㡣һ��unsigned char������8λ����1���ֽڣ����е��ַ�����������8λ����ʾ��һ��256���ַ����࣬����������i��Ӧ��Ӧ���ַ����࣬Ϊ�˺����ͳ���ַ����෽��
	for (i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].uch = (unsigned char)i; // �����256���±���256���ַ���Ӧ��Ϊ�˺���ͳ���ַ����������
	}

	infile = fopen(ifname, "rb"); //infile�Ǹ��ļ�ָ��(FILE*).FilefullName��ifname���Ǹ��ļ���,�������ַ���(char*).rb��ʾ�ǿ��ɶ����ƴ򿪲������ļ���r��ʾread�����ļ�
	// �ж������ļ��Ƿ����
	if (infile == NULL) //�����ʧ�ܣ���infile���ؿ�ָ��
		return -1;
	fread((char*)&char_temp, sizeof(unsigned char), 1, infile); //��infile�ж���1��sizeof(unsigned char)������С���ֽ�
	// �����ļ�����ȡ�ַ�Ƶ��
	while (!feof(infile))
	{
		++tmp_nodes[char_temp].weight;								 // ͳ���±��Ӧ�ַ���Ȩ�أ����������������ʿ���ͳ���ַ�Ƶ�ȣ���Ϊtmp_nodes[i].uch = (unsigned char)i;�����256���±���256���ַ���Ӧ�����Կ���ֱ�����±��Ӧ���ַ�����
		++file_len;													 //ÿ����һ���ַ����ļ��ַ��ܳ��ȼ�1���ó��ַ��ܳ���
		fread((char*)&char_temp, sizeof(unsigned char), 1, infile); //��infile�ж���1��sizeof(unsigned char)������С���ֽ�
	}
	fclose(infile); //���꣬�ر��ļ�

	// ������ҪĿ���ǽ�Ƶ��Ϊ��ķ���󣬽�����һ�����޳�������a[0]��a[9],�����м��м����ַ���û�г��ֹ���������Ҫ����ЩƵ��Ϊ0���ַ��ŵ����ȥ����֤ǰ�涼�ǳ��ֹ����ַ�
	for (i = 0; i < 256 - 1; ++i)
		for (j = i + 1; j < 256; ++j)
			if (tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				node_temp = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = node_temp;
			}
	// ͳ��ʵ�ʵ��ַ����ࣨ���ִ�����Ϊ0��
	for (i = 0; i < 256; ++i)
		if (tmp_nodes[i].weight == 0) //һֱѭ����Ƶ��Ϊ0ʱ����i�ͱ�ʾ�ַ�������������Ϊ��һ���Ѿ���Ƶ��Ϊ0�ķŵ��˺���
			break;
	char_kinds = i; //i�ͱ�ʾ���ַ�������

	if (char_kinds == 1) //���ֻ��һ���ַ�
	{
		outfile = fopen(ofname, "wb");											 // ��ѹ�������ɵ��ļ�
		fwrite((char*)&char_kinds, sizeof(unsigned int), 1, outfile);			 // д���ַ�����
		fwrite((char*)&tmp_nodes[0].uch, sizeof(unsigned char), 1, outfile);	 // д��Ψһ���ַ�
		fwrite((char*)&tmp_nodes[0].weight, sizeof(unsigned long), 1, outfile); // д���ַ�Ƶ�ȣ�Ҳ�����ļ�����
		delete[]tmp_nodes;//free(tmp_nodes);
		fclose(outfile);
	}
	else //������಻ֻһ���ַ�����
	{
		node_num = 2 * char_kinds - 1;							  // �����ַ������������㽨���������������������൱��m=2*n-1
		huf_tree = new HufNode[node_num];//(HufNode*)malloc(node_num * sizeof(HufNode)); // ��̬������������������

		// ��ʼ��ǰchar_kinds�����
		for (i = 0; i < char_kinds; ++i)
		{
			// ���ݴ�����ַ���Ƶ�ȿ����������
			huf_tree[i].uch = tmp_nodes[i].uch;
			huf_tree[i].weight = tmp_nodes[i].weight;
			huf_tree[i].parent = 0;
		}
		delete[]tmp_nodes;//free(tmp_nodes); // �ͷ��ַ�Ƶ��ͳ�Ƶ��ݴ���

		// ��ʼ����node_num-char_kins�����
		for (; i < node_num; ++i)
			huf_tree[i].parent = 0;

		CreateTree(huf_tree, char_kinds, node_num); // ������������

		HufCode(huf_tree, char_kinds); // ���ɹ���������

		// д���ַ�����ӦȨ�أ�����ѹʱ�ؽ���������
		outfile = fopen(ofname, "wb"); // ��ѹ�������ɵ��ļ�
		fwrite((char*)&char_kinds, sizeof(unsigned int), 1, outfile);
		// д���ַ�����size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
		//ע�⣺��������Զ�������ʽ���ļ����в��������������ı��ļ�
		//����ֵ������ʵ��д������ݿ���Ŀ
		//��1��buffer����һ��ָ�룬��fwrite��˵����Ҫ��ȡ���ݵĵ�ַ��
		//��2��size��Ҫд�����ݵĵ��ֽ�����
		//��3��count:Ҫ����д��size�ֽڵ�������ĸ�����
		//��4��stream:Ŀ���ļ�ָ�룻
		//��5������ʵ��д������������count��
		for (i = 0; i < char_kinds; ++i) //����������Ϊ�˰�˳��д��������ļ�
		{
			fwrite((char*)&huf_tree[i].uch, sizeof(unsigned char), 1, outfile);	// д���ַ��������򣬶�����˳�򲻱䣩
			fwrite((char*)&huf_tree[i].weight, sizeof(unsigned long), 1, outfile); // д���ַ���ӦȨ��
		}

		// �������ַ���Ȩ����Ϣ����д���ļ����Ⱥ��ַ�����
		fwrite((char*)&file_len, sizeof(unsigned long), 1, outfile); // д���ļ�����
		infile = fopen(ifname, "rb");								  // �Զ�������ʽ�򿪴�ѹ�����ļ�
		fread((char*)&char_temp, sizeof(unsigned char), 1, infile);  // ÿ�ζ�ȡ8bits
		while (!feof(infile))										  //��û�ж����ļ�ĩβ��ʱ��
		{
			// ƥ���ַ���Ӧ����
			for (i = 0; i < char_kinds; ++i)
				if (char_temp == huf_tree[i].uch)
					strcat(code_buf, huf_tree[i].code);
			//�μ���http://www.kuqin.com/clib/string/strcat.html
			//ԭ�ͣ�extern char *strcat(char *dest,char *src);
			//�÷���#include <string.h>
			//���ܣ���src��ָ�ַ�����ӵ�dest��β��(����dest��β����'\0')�����'\0'��
			//˵����src��dest��ָ�ڴ����򲻿����ص���dest�������㹻�Ŀռ�������src���ַ�����
			//      ����ָ��dest��ָ�롣

			// ��8λ��һ���ֽڳ��ȣ�Ϊ����Ԫ
			while (strlen(code_buf) >= 8) //��Ϊ����8bit���ж�ȡ��
			{
				char_temp = '\0'; // ����ַ��ݴ�ռ䣬��Ϊ�ݴ��ַ���Ӧ����
				for (i = 0; i < 8; ++i)
				{
					char_temp <<= 1; // ����һλ��Ϊ��һ��bit�ڳ�λ�ã����������a=a*2+(str[i]-'0').(str[i]-'0')�ǽ�һ��char����ת���������͡�a=a*2�ο���321=����3*10+2��*10+1������ʵ���Ǵ��������мӷ���ʱ�򣬽�string��ÿһ���ַ���������
					if (code_buf[i] == '1')
						char_temp |= 1; // ������Ϊ"1"��ͨ���������������ӵ��ֽڵ����λ��   |= 1��ʾ��1���㡣�൱������һ����321�����+2��+1�Ĺ���
				}
				fwrite((char*)&char_temp, sizeof(unsigned char), 1, outfile); // ���ֽڶ�Ӧ��������ļ�
				strcpy(code_buf, code_buf + 8);								   // ���뻺��ȥ���Ѵ����ǰ��λ
			}
			fread((char*)&char_temp, sizeof(unsigned char), 1, infile); // ÿ�ζ�ȡ8bits
		}
		// ���������8bits����
		code_len = strlen(code_buf);
		if (code_len > 0) //���������ж���Ϊ�˷�ֹ��һ������ÿ8λ��ȡʱ�����ó�����8�ı�����
		{
			char_temp = '\0';
			for (i = 0; i < code_len; ++i)
			{
				char_temp <<= 1;
				if (code_buf[i] == '1')
					char_temp |= 1;
			}
			char_temp <<= 8 - code_len;									   // �������ֶδ�β���Ƶ��ֽڵĸ�λ����0����
			fwrite((char*)&char_temp, sizeof(unsigned char), 1, outfile); // �������һ���ֽ�
		}
		// �ر��ļ�
		fclose(infile);
		fclose(outfile);
		// �ͷ��ڴ�
		for (i = 0; i < char_kinds; ++i)
			delete huf_tree[i].code;//free(huf_tree[i].code);
		delete[]huf_tree;//free(huf_tree);
	}
}

int extract(char* ifname, char* ofname)
{
	unsigned int i;
	unsigned long file_len;
	unsigned long writen_len = 0; // �����ļ�д�볤��
	FILE* infile, * outfile;
	unsigned int char_kinds; // �洢�ַ�����
	unsigned int node_num;
	HufNode * huf_tree;
	unsigned char code_temp; // �ݴ�8bits����
	unsigned int root;		 // ������ڵ���������ƥ�����ʹ��

	infile = fopen(ifname, "rb"); // �Զ����Ʒ�ʽ��ѹ���ļ�
	// �ж������ļ��Ƿ����
	if (infile == NULL)
		return -1;

	// ��ȡѹ���ļ�ǰ�˵��ַ�����Ӧ���룬�����ؽ���������
	fread((char*)&char_kinds, sizeof(unsigned int), 1, infile); // ��ȡ�ַ�������
	if (char_kinds == 1)										 //���ֻ��һ���ַ�
	{
		fread((char*)&code_temp, sizeof(unsigned char), 1, infile); // ��ȡΨһ���ַ�
		fread((char*)&file_len, sizeof(unsigned long), 1, infile);	 // ��ȡ�ļ�����
		outfile = fopen(ofname, "wb");								 // ��ѹ�������ɵ��ļ�
		while (file_len--)
			fwrite((char*)&code_temp, sizeof(unsigned char), 1, outfile);
		fclose(infile);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;							  // �����ַ������������㽨������������������
		huf_tree = new HufNode[node_num];//(HufNode*)malloc(node_num * sizeof(HufNode)); // ��̬��������������ռ�
		// ��ȡ�ַ�����ӦȨ�أ�������������ڵ�
		for (i = 0; i < char_kinds; ++i)
		{
			fread((char*)&huf_tree[i].uch, sizeof(unsigned char), 1, infile);	  // �����ַ�
			fread((char*)&huf_tree[i].weight, sizeof(unsigned long), 1, infile); // �����ַ���ӦȨ��
			huf_tree[i].parent = 0;
		}
		// ��ʼ����node_num-char_kins������parent
		for (; i < node_num; ++i)
			huf_tree[i].parent = 0;

		CreateTree(huf_tree, char_kinds, node_num); // �ؽ�������������ѹ��ʱ��һ�£�

		// �����ַ���Ȩ����Ϣ�������Ŷ�ȡ�ļ����Ⱥͱ��룬���н���
		fread((char*)&file_len, sizeof(unsigned long), 1, infile); // �����ļ�����
		outfile = fopen(ofname, "wb");								// ��ѹ�������ɵ��ļ�
		root = node_num - 1;
		while (1)
		{
			fread((char*)&code_temp, sizeof(unsigned char), 1, infile); // ��ȡһ���ַ����ȵı���

			// �����ȡ��һ���ַ����ȵı��루ͨ��Ϊ8λ��
			for (i = 0; i < 8; ++i)
			{
				// �ɸ�����ֱ��Ҷ�ڵ�����ƥ������Ӧ�ַ�
				if (code_temp & 128)
					root = huf_tree[root].rchild;
				else
					root = huf_tree[root].lchild;

				if (root < char_kinds)
				{
					fwrite((char*)&huf_tree[root].uch, sizeof(unsigned char), 1, outfile);
					++writen_len;
					if (writen_len == file_len)
						break;			 // �����ļ����ȣ������ڲ�ѭ��
					root = node_num - 1; // ��λΪ��������ƥ����һ���ַ�
				}
				code_temp <<= 1; // �����뻺�����һλ�Ƶ����λ����ƥ��
			}
			if (writen_len == file_len)
				break; // �����ļ����ȣ��������ѭ��
		}

		// �ر��ļ�
		fclose(infile);
		fclose(outfile);
		// �ͷ��ڴ�
		delete[]huf_tree;//free(huf_tree);
	}
}