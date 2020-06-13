#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <iostream>
using namespace std;

class Point //坐标数据结构
{
public:
	double x;
	double y;
};

// 统计字符频度的临时结点
class TmpNode
{
public:
	unsigned char uch;	  // 以8bits为单元的无符号字符，定义uch是为了统计字符种类和进行字符种类位置交换来用
	unsigned long weight; // 每类（以二进制编码区分）字符出现频度，即权值
};

// 哈夫曼树结点
class HufNode
{
public:
	unsigned char uch;			// 以8bits为单元的无符号字符
	unsigned long weight;		// 每类（以二进制编码区分）字符出现频度，即权值
	char* code;					// 字符对应的哈夫曼编码（动态分配存储空间）
	int parent, lchild, rchild; // 定义双亲和左右孩子
	double idx, idy;
	Point p;
};

void select(HufNode* huf_tree, unsigned int n, int* s1, int* s2);
void CreateTree(HufNode* huf_tree, unsigned int char_kinds, unsigned int node_num);
void HufCode(HufNode* huf_tree, unsigned char_kinds);
int compress(char* ifname, char* ofname);
int extract(char* ifname, char* ofname);
void drawing();