
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <functional>
#include "ThreadPool.h"
#include "FileUtil.h"
#include<windows.h>
#include <mutex>
#include <map>

using namespace std;

FileItem fileItem;
LineInfo lineInfo;

void getResultItem(const string &perPath,  LineInfo &lineInfo) {
	FileUtil::getFileNameAndExt(perPath, fileItem);
	string fileName = fileItem.fileName;
	string ext = fileItem.extName;
	bool isTextFile = FileUtil::isTextFile(ext);
	int textFileLineQuantity = 0;
	if (isTextFile) {
		textFileLineQuantity = FileUtil::lineQuantity(perPath);
	}
	lineInfo.ext = ext;
	lineInfo.lineCount = textFileLineQuantity;
}


int main() {
	FileUtil::initConfig();

	ThreadPool threadPool(20);
	mutex lock;
	char dir[200];
	cout << "code project path: ";
	cin.getline(dir, 200);
	vector<string>allPath = FileUtil::getFilesList(dir);
	map<string,int> lineMap;
	map<string,int> fileMap;
	auto count = allPath.size();
	//cout << count << endl;
	//cout << "输出所有文件的路径：" << endl;
	for (int i = 0; i < count; i++)
	{
		string perPath = allPath.at(i);
		
		threadPool.execute([perPath,&lineMap,&fileMap,&count, &lock]() {
			getResultItem(perPath, lineInfo);
			lock.lock();
			string ext = lineInfo.ext;
			int lineCount = lineInfo.lineCount;
			if (lineMap.count(ext) >= 1) {
				lineMap[ext] = lineMap.at(ext) + lineCount;
			}
			else {
				lineMap[ext] = lineCount;
			}
			if (fileMap.count(ext) >= 1) {
				fileMap[ext] = fileMap.at(ext) + 1;
			}
			else {
				fileMap[ext] = 1;
			}
			lock.unlock();
		});
	}

	while (threadPool.taskCount() > 0) {
		Sleep(500);
		//cout << "剩余任务数量：" << threadPool.taskCount() << endl;
	}
	//排序输出
	typedef pair<string, int> PAIR;

	vector<PAIR> mapPairList;
	for (auto iter = lineMap.begin(); iter != lineMap.end(); iter++) {
		mapPairList.push_back(*iter);
	}
	sort(mapPairList.begin(), mapPairList.end(), [](const PAIR& a, const PAIR& b) {
		return b.second < a.second;
	});

	cout << "fileType\t" << "fileCount\t" << "lineCount" << endl;
	for (auto item : mapPairList) {
		cout << item.first + "\t\t" << to_string(fileMap.at(item.first)) + "\t\t" << item.second<< endl;
	}

	system("pause");
	return 0;
}

