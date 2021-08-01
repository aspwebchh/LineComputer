
#include <vector>
#include <string>
#include <iostream>
#include "ThreadPool.h"
#include "FileUtil.h"
#include <windows.h>
#include <mutex>
#include <map>

using namespace std;

map<string, int> lineMap;
map<string, int> fileMap;
mutex mlock;


int main() {
	FileUtil::initConfig();

	ThreadPool threadPool(50);
	char dir[1024];
	cout << "code project path: ";
	cin.getline(dir, 200);
	vector<string>allPath;
	FileUtil::getFilesList(dir, allPath);
	auto count = allPath.size();

	for (int i = 0; i < count; i++)
	{
		string perPath = allPath.at(i);
		FileItem fileItem;
		FileUtil::getFileNameAndExt(perPath, fileItem);
		bool isTextFile = FileUtil::isTextFile(fileItem.extName);
		if (!isTextFile) {
			continue;
		}
		
		threadPool.execute([perPath,fileItem]() {
			int lineCount = FileUtil::lineQuantity(perPath);
			string ext = fileItem.extName;
			mlock.lock();
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
			mlock.unlock();
		});
	}

	while (threadPool.taskCount() > 0) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		Sleep(500);
	}
	//≈≈–Ú ‰≥ˆ
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

