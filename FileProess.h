#pragma once
#include "CreateHuffman.h"
#include <fstream>
#include <string>
#include <assert.h>
typedef long long LongType;
struct CharInfo
{
	char _ch;   //字符
	LongType _count; //权值(出现次数)，8个字节
	string _code;  //对应编码
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo info_;
		info_._count = _count + info._count;
		return info_;
	}
	bool operator>(const CharInfo& info)  const
	{
		return _count > info._count;
	}
	bool operator!=(const CharInfo& info) const
	{
		return _count != info._count;
	}
	~CharInfo()
	{}
};
class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
	struct ConfigInfo
	{
		char _ch;
		LongType _count;
	};
public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; i++){
			_hashInfos[i]._count = 0;
			_hashInfos[i]._ch = i;
		}
	}
	void GenerateHuffmanCode(Node* root)
	{
		if (root == nullptr)
			return;
		if (root->_left == nullptr && root->_right == nullptr){
			_hashInfos[(unsigned char)root->_data._ch]._code = root->_data._code;
			return;
		}
		Node* left = root->_left;
		if (left){
			left->_data._code = root->_data._code + '0';  //string消耗太大
			GenerateHuffmanCode(root->_left);
		}
		Node* right = root->_right;
		if (right){
			right->_data._code = root->_data._code + '1';
			GenerateHuffmanCode(root->_right);
		}
	}
	//void GenerateHuffmanCode(Node* root)
	//{
	//	if (root == nullptr)
	//		return;
	//	if (root->_left == nullptr && root->_right == nullptr){  //如果它是叶子
	//		string& code = _hashInfos[root->_data._ch]._code;
	//		Node* cur = root;
	//		Node* parent = cur->_parent;
	//		while (parent){
	//			if (cur == parent->_left)
	//				code += '0';
	//			else
	//				code += '1';
	//			cur = parent;
	//			parent = cur->_parent;
	//		}
	//		reverse(code.begin(), code.end());   //逆置
	//	}
	//	GenerateHuffmanCode(root->_left);
	//	GenerateHuffmanCode(root->_right);
	//}
	void Compress(const char* file)    //压缩文件
	{
		//1.统计字符出现的次数
		ifstream ifs(file,ios_base::in | ios_base::binary);  //打开文件
		char ch;             //-127 --- +128
		while (ifs.get(ch)){   //获取字符
			_hashInfos[(unsigned char)ch]._count++;
		}
		//2.生成哈夫曼树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);
		//3.编码
		GenerateHuffmanCode(tree.GetRoot());
		//4.压缩
		string compressfile = file;
		size_t pos_ = compressfile.rfind(".txt");   //找到.txt
		assert(pos_ != string::npos);
		compressfile.erase(pos_);
		compressfile += "_Huffman.txt";              //替换为"Input_Huffman.txt"
		ofstream ofs(compressfile.c_str(), ios_base::in | ios_base::binary);
		//5.往压缩文件中写入字符出现的次数,不然解压缩的时候无法构建Huffman树
		for (size_t i = 0; i < 256; i++){
			if (_hashInfos[i]._count > 0){
				//往压缩文件中写CharInfo结构体
				ConfigInfo info;
				info._ch = _hashInfos[i]._ch;
				info._count = _hashInfos[i]._count;
				ofs.write((char*)&info, sizeof(info));
			}
		}
		ConfigInfo end;
		end._count = 0;
		ofs.write((char*)&end, sizeof(end));   //在最后写入一个_count = 0的CharInfo标记结尾
		char value = 0;
		int pos = 0;
		int n = 0;
		ifs.clear();        //清理一下
		ifs.seekg(0);       //刚才已经读完了,现在重新设置到开始
		while (ifs.get(ch)){
			++n;
			string& code = _hashInfos[(unsigned char)ch]._code;
			for (size_t i = 0; i < code.size(); i++){  //小端机,数据低位在低地址,高位在高地址
				if (code[i] == '0'){
					value &= ~(1 << pos);   //置pos位为0
				}
				else if (code[i] == '1'){
					value |= 1 << pos;     //置pos位为1
				}
				else{
					assert(0);
				}
				pos++;
				if (pos == 8){
					ofs.put(value);
					//printf("%x ", value);
					pos = 0;
					value = 0;
				}
			}
		}
		if (pos > 0){
			ofs.put(value);     //将最后没置满位的一个字节写入进去
			//printf("%x ", value);
		}
		cout << "源文件： " <<n << endl;
	}
	void UnCompress(const char* file)    //解压缩
	{
		//1.打开压缩文件
		ifstream ifs(file,ios_base::in | ios_base::binary);
		string uncompressfile = file;
		size_t pos = uncompressfile.rfind("_Huffman"); //文件名"Input_Huffman.txt"
		assert(pos != string::npos);
		uncompressfile.erase(pos);        //删掉"_Huffman.txt"
#ifdef _DEBUG
		uncompressfile += "_Unhuffman.txt";   //加上".unhuffman.txt"
#endif
		ofstream ofs(uncompressfile.c_str(), ios_base::in | ios_base::binary);
		//2.重建huffman树
		int n = 0;
		while (1){
			ConfigInfo info;
			ifs.read((char*)&info, sizeof(info));
			if (info._count > 0){
				_hashInfos[(unsigned char)info._ch]._count = info._count;
				n += info._count;
			}
			else
				break;
		}
		cout << "读到：" << n << endl;
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);
		//3.解压缩
		Node* root = tree.GetRoot();
		LongType filecount = root->_data._count;   //所有字符出现的次数,就是根节点的权值
		Node* cur = root;
		char ch;
		while (ifs.get(ch)){
			for (size_t pos = 0; pos < 8; ++pos){
				if (ch & (1 << pos)){   //pos为1
					cur = cur->_right;
				}
				else             //pos位为0
				{
					cur = cur->_left;
				}
				if (cur->_left == nullptr && cur->_right == nullptr){
					ofs.put(cur->_data._ch);
					cur = root;
					if (--filecount == 0)
						break;
				}
			}
		}
	}
	~FileCompress()
	{}
private:
	CharInfo _hashInfos[256];
};

void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("Input.txt");      //要压缩的文件路径
}
void TestFileUnCompress()
{
	FileCompress fc;
	fc.UnCompress("E:\\CProject\\数据结构\\文件压缩\\Input_Huffman.txt");        //要解压的文件路径
}