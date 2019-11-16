#pragma once

int faceDetect(const std::string& access_token, const std::string& imageStr, size_t(*pFaceDetectCallback)(void*, size_t, size_t, void*),sLoginSock* lthis);
int addFace(const std::string& access_token, const std::string& imageStr, const std::string account, size_t(*pAddFaceCallback)(void*, size_t, size_t, void*), sLoginSock* lthis);
int faceSearch(const std::string& access_token, const std::string& imageStr, size_t(*pFaceSearchCallback)(void*, size_t, size_t, void*), sLoginSock* lthis);
const std::string accessToken = "24.f7e4781e19be9f9545a167618acb7e1a.2592000.1574679301.282335-17625142";
