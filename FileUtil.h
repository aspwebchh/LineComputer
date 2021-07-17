#include <vector>
#include <string>
#include <iostream> //for cout
#include <io.h>     //for _finddata_t�� _findnext
#include <fstream>
#include <cassert>
#include <regex>
#include <tuple>
#include <map>
#include <list>
#include <regex>
#include "structs.h"

using namespace std;

static vector<string> * exts = new vector<string>;

class FileUtil {

public:

	static void initConfig() {
		auto text = FileUtil::readText("config.conf");
		FileUtil::splitAndTrim(text, *exts, ",");
	}

	static bool isBlank(std::string& s)
	{
		if (s.empty()) {
			return true;
		}
		for (int i = 0; i < s.size(); i++) {
			auto item = s[i];
			if (item != ' ' && item != '	') {
				return false;
			}
		}
		return true;
	}

	static string trim(string str) {
		std::regex reg("^(\\s*)|(\\s*)$");
	    return std::regex_replace(str, reg,"");
	}


	static void splitAndTrim(const string& s, vector<string>& tokens, const string& delimiters = " ") {
		string::size_type lastPos = s.find_first_not_of(delimiters, 0); 
		string::size_type pos = s.find_first_of(delimiters, lastPos); 
		while (string::npos != pos || string::npos != lastPos) {
			auto item = s.substr(lastPos, pos - lastPos);
			tokens.push_back(trim(item));//use emplace_back after C++11 
			lastPos = s.find_first_not_of(delimiters, pos);
			pos = s.find_first_of(delimiters, lastPos);
		}
	}

	static bool isTextFile(const string &ext) {
		for (auto extItem : *exts) {
			if (extItem == ext) {
				return true;
			}
		}
		return false;
	}

	static void getFileNameAndExt(const string& path, FileItem& fileItem) {
		vector<string> results;
		FileUtil::splitAndTrim(path, results, "\\");
	
		string fileName = results[results.size() - 1];
		results.clear();

		fileItem.fileName = fileName;

		FileUtil::splitAndTrim(fileName, results, ".");
		if (results.size() == 2) {
			fileItem.extName = results[1];
		}
		else {
			fileItem.extName = "";
		}
	}

	static int lineQuantity(const string &filePath) {
		ifstream infile;
		infile.open(filePath.data());   //���ļ����������ļ��������� 
		assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 

		int quantity = 0;
		string line;
		while (getline(infile, line)) {
			if (isBlank(line)) {
				continue;
			}
			quantity++;
		}
		infile.close();
		return quantity;
	}

	static string readText(const string &filePath) {
		ifstream infile;
		infile.open(filePath.data());   //���ļ����������ļ��������� 
		assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 

		string result;
		string s;
		while (getline(infile, s))
		{
			result += s + "\n";
		}
		infile.close();
		return result;
	}

	static vector<string> getFilesList(const string &dir)
	{
		vector<string> allPath;
		// ��Ŀ¼�������"\\*.*"���е�һ������
		string dir2 = dir + "\\*.*";

		intptr_t handle;
		_finddata_t findData;

		handle = _findfirst(dir2.c_str(), &findData);
		if (handle == -1) {// ����Ƿ�ɹ�
			cout << "can not found the file ... " << endl;
			return allPath;
		}
		do
		{
			if (findData.attrib & _A_SUBDIR) //�Ƿ�����Ŀ¼
			{
				//������Ŀ¼Ϊ"."��".."���������һ��ѭ�������������Ŀ¼������������һ������
				if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
					continue;

				// ��Ŀ¼�������"\\"����������Ŀ¼��������һ������
				string dirNew = dir + "\\" + findData.name;
				vector<string> tempPath = getFilesList(dirNew);
				allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());
			}
			else //������Ŀ¼�������ļ���������ļ������ļ��Ĵ�С
			{
				string filePath = dir + "\\" + findData.name;
				allPath.push_back(filePath);
				//cout << filePath << "\t" << findData.size << " bytes.\n";
			}
		} while (_findnext(handle, &findData) == 0);
		_findclose(handle);    // �ر��������
		return allPath;
	}
};