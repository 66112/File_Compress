/*
1.统计字符出现的个数 a 4 ; b 3; c 2; d 1
2.构建huffman树，贪心算法
3.生成huffman编码  左0 右1
4.压缩
5.解压缩
本质:将一个一个字符,转换成位，1个字节对应8个位
*/
#include <iostream>
#include "FileProess.h"
using namespace std;
int main()
{
	TestFileCompress();
	//TestFileUnCompress();
	return 0;
}