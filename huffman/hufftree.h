#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <iostream>
using namespace std;

class Point //�������ݽṹ
{
public:
	double x;
	double y;
};

// ͳ���ַ�Ƶ�ȵ���ʱ���
class TmpNode
{
public:
	unsigned char uch;	  // ��8bitsΪ��Ԫ���޷����ַ�������uch��Ϊ��ͳ���ַ�����ͽ����ַ�����λ�ý�������
	unsigned long weight; // ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ�ȣ���Ȩֵ
};

// �����������
class HufNode
{
public:
	unsigned char uch;			// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long weight;		// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ�ȣ���Ȩֵ
	char* code;					// �ַ���Ӧ�Ĺ��������루��̬����洢�ռ䣩
	int parent, lchild, rchild; // ����˫�׺����Һ���
	double idx, idy;
	Point p;
};

void select(HufNode* huf_tree, unsigned int n, int* s1, int* s2);
void CreateTree(HufNode* huf_tree, unsigned int char_kinds, unsigned int node_num);
void HufCode(HufNode* huf_tree, unsigned char_kinds);
int compress(char* ifname, char* ofname);
int extract(char* ifname, char* ofname);
void drawing();