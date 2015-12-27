/**
 *  @author        Ryan Huang <ryanhuang@cs.ucsd.edu>
 *  @organization  University of California, San Diego
 * 
 * Tachyon C/C++ APIs
 *
 */

#include "Tachyon.h"
#include "Util.h"

#include <string>
#include <string.h>
#include <stdlib.h>

using namespace tachyon;
using namespace tachyon::jni;

jTachyonClient TachyonClient::createClient(const char *masterUri)
{
  Env env;
  jstring jPathStr; 
  jvalue ret;

  jPathStr = env.newStringUTF(masterUri, "masterUri");
  // might throw exception, caller needs to handle it
  env.callStaticMethod(&ret, TFS_CLS, TFS_GET_METHD, 
                "(Ljava/lang/String;)Ltachyon/client/TachyonFS;", jPathStr);
  
  env->DeleteLocalRef(jPathStr); 
  return new TachyonClient(env, ret.l);
}

jTachyonClient TachyonClient::copyClient(jTachyonClient client)
{
  return new TachyonClient(client->getEnv(), client->getJObj());
}

jTachyonFile TachyonClient::getFile(const char * path)
{
  jvalue ret;
  jstring jPathStr;
  
  jPathStr = m_env.newStringUTF(path, "path");
  m_env.callMethod(&ret, m_obj, TFS_GET_FILE_METHD, 
                "(Ljava/lang/String;)Ltachyon/client/TachyonFile;", jPathStr);
  m_env->DeleteLocalRef(jPathStr); 
  if (ret.l == NULL)
    return NULL;
  return new TachyonFile(m_env, ret.l);
}

jTachyonFile TachyonClient::getFile(int fid)
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFS_GET_FILE_METHD, 
                "(I)Ltachyon/client/TachyonFile;", (jint) fid);
  if (ret.l == NULL)
    return NULL;
  return new TachyonFile(m_env, ret.l);
}

jTachyonFile TachyonClient::getFile(int fid, bool useCachedMetadata)
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFS_GET_FILE_METHD, 
                "(IZ)Ltachyon/client/TachyonFile;",
                (jint) fid, (jboolean) useCachedMetadata);
  if (ret.l == NULL)
    return NULL;
  return new TachyonFile(m_env, ret.l);
}

int TachyonClient::getFileId(const char *path)
{
  jvalue ret;
  jTachyonURI uri = TachyonURI::newURI(path);
  if (uri == NULL)
    return -1;
  m_env.callMethod(&ret, m_obj, TFS_GET_FILEID_METHD, 
                    "(Ltachyon/TachyonURI;)I", uri->getJObj());
  delete uri;
  return ret.i;
}

int TachyonClient::createFile(const char * path)
{
  jvalue ret;
  jstring jPathStr;
  
  jPathStr = m_env.newStringUTF(path, "path");

  m_env.callMethod(&ret, m_obj, TFS_CREATE_FILE_METHD, 
                    "(Ljava/lang/String;)I", jPathStr);
  m_env->DeleteLocalRef(jPathStr); 
  return ret.i;
}

bool TachyonClient::mkdir(const char *path)
{
  jvalue ret;
  jTachyonURI uri = TachyonURI::newURI(path);
  if (uri == NULL)
    return false;
  
  m_env.callMethod(&ret, m_obj, TFS_MKDIR_METHD, 
                "(Ltachyon/TachyonURI;)Z", uri->getJObj());
  delete uri;
  return ret.z;
}

bool TachyonClient::mkdirs(const char *path, bool recursive)
{
  jvalue ret;
  jTachyonURI uri = TachyonURI::newURI(path);
  if (uri == NULL)
    return false;
  
  m_env.callMethod(&ret, m_obj, TFS_MKDIRS_METHD, 
                "(Ltachyon/TachyonURI;Z)Z", uri->getJObj(), (jboolean) recursive);
  delete uri;
  return ret.z;
}

bool TachyonClient::deletePath(const char *path, bool recursive)
{
  jvalue ret;
  jstring jPathStr;
  
  jPathStr = m_env.newStringUTF(path, "path");
  m_env.callMethod(&ret, m_obj, TFS_DELETE_FILE_METHD, 
                    "(Ljava/lang/String;Z)Z", jPathStr, (jboolean) recursive);
  m_env->DeleteLocalRef(jPathStr); 
  return ret.z;
}

bool TachyonClient::deletePath(int fid, bool recursive)
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFS_DELETE_FILE_METHD, 
                    "(IZ)Z", (jint) fid, (jboolean) recursive);
  return ret.z;
}

long TachyonFile::length()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_LENGTH_METHD, "()J");
  return ret.j;
}

bool TachyonFile::isFile()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_ISFILE_METHD, "()Z");
  return ret.z;
}

bool TachyonFile::isComplete()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_ISCOMPLETE_METHD, "()Z");
  return ret.z;
}

bool TachyonFile::isDirectory()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_ISDIRECTORY_METHD, "()Z");
  return ret.z;
}

bool TachyonFile::isInMemory()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_ISINMEMORY_METHD, "()Z");
  return ret.z;
}

bool TachyonFile::needPin()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_NEEDPIN_METHD, "()Z");
  return ret.z;
}

bool TachyonFile::recache()
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_RECACHE_METHD, "()Z");
  return ret.z;
}

char * TachyonFile::getPath()
{
  jvalue ret;
  jstring jpath;
  const char *path;
  char *retPath;

  m_env.callMethod(&ret, m_obj, TFILE_PATH_METHD, "()Ljava/lang/String;");
  if (ret.l == NULL)
    return NULL;
  jpath = (jstring) ret.l;
  path = m_env->GetStringUTFChars(jpath, 0);
  retPath = strdup(path);
  m_env->ReleaseStringUTFChars(jpath, path);
  return retPath;
}

jTachyonByteBuffer TachyonFile::readByteBuffer(int blockIndex)
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TFILE_RBB_METHD, 
                "(I)Ltachyon/client/TachyonByteBuffer;", (jint) blockIndex);
  if (ret.l == NULL)
    return NULL;
  return new TachyonByteBuffer(m_env, ret.l);
}

jInStream TachyonFile::getInStream(ReadType readType)
{
  jvalue ret;
  jobject eobj;

  eobj = enumObjReadType(m_env, readType);
  m_env.callMethod(&ret, m_obj, TFILE_GIS_METHD, 
                "(Ltachyon/client/ReadType;)Ltachyon/client/InStream;", eobj);
  if (ret.l == NULL)
    return NULL;
  return new InStream(m_env, ret.l);
}

jOutStream TachyonFile::getOutStream(WriteType writeType)
{
  jvalue ret;
  jobject eobj;

  eobj = enumObjWriteType(m_env, writeType);
  m_env.callMethod(&ret, m_obj, TFILE_GOS_METHD, 
                "(Ltachyon/client/WriteType;)Ltachyon/client/OutStream;", eobj);
  if (ret.l == NULL)
    return NULL;
  return new OutStream(m_env, ret.l);
}

jByteBuffer TachyonByteBuffer::getData()
{
  jobject ret;
  ret = m_env.getObjectField(m_obj, "mData", "Ljava/nio/ByteBuffer;");
  return new ByteBuffer(m_env, ret);
}

void TachyonByteBuffer::close()
{
  m_env.callMethod(NULL, m_obj, TBBUF_CLOSE_METHD, "()V");
}

jByteBuffer ByteBuffer::allocate(int capacity)
{
  Env env;
  jvalue ret;
  env.callStaticMethod(&ret, BBUF_CLS, BBUF_ALLOC_METHD, 
                "(I)Ljava/nio/ByteBuffer;", (jint) capacity);
  if (ret.l == NULL)
    return NULL;
  return new ByteBuffer(env, ret.l);
}

//////////////////////////////////////////
//InStream
//////////////////////////////////////////

int InStream::readImpl() 
{
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TISTREAM_READ_METHD, "()I");
  return ret.i;
}

int InStream::readImpl(void *buff, int length, int off, int maxLen)
{
  jbyteArray jBuf;
  jvalue ret;
  int rdSz;

  try {
    jBuf = m_env.newByteArray(length);
    if (off < 0 || maxLen <= 0 || length == maxLen)
      m_env.callMethod(&ret, m_obj, TISTREAM_READ_METHD, "([B)I", jBuf);
    else
      m_env.callMethod(&ret, m_obj, TISTREAM_READ_METHD, "([BII)I", jBuf, off, maxLen);
  } catch (NativeException) {
    if (jBuf != NULL) {
      m_env->DeleteLocalRef(jBuf);
    }
    throw;
  }
  rdSz = ret.i;
  if (rdSz > 0) {
    m_env->GetByteArrayRegion(jBuf, 0, length, (jbyte*) buff);
  }
  m_env->DeleteLocalRef(jBuf);
  return rdSz;
}

void InStream::closeImpl()
{
  m_env.callMethod(NULL, m_obj, TISTREAM_CLOSE_METHD, "()V");
}

void InStream::seekImpl(long pos)
{
  m_env.callMethod(NULL, m_obj, TISTREAM_SEEK_METHD, "(J)V", (jlong) pos);
}

long InStream::skipImpl(long n)
{
  jvalue ret;
  m_env.callMethod(NULL, m_obj, TISTREAM_SKIP_METHD, "(J)J", (jlong) n);
  return ret.j;
}


// Call the templates

int InStream::read()
{
  return readImpl();
}

int InStream::read(void *buff, int length, int off, int maxLen) 
{
  return readImpl(buff, length, off, maxLen);
}

int InStream::read(void *buff, int length)
{
  return read(buff, length, 0, length);
}

void InStream::close()
{
  closeImpl();
}

void InStream::seek(long pos)
{
  seekImpl(pos);
}

long InStream::skip(long n)
{
  return skipImpl(n);
}

//////////////////////////////////////////
// FileInStream
//////////////////////////////////////////

int FileInStream::read()
{
  return readImpl();
}

int FileInStream::read(void *buff, int length, int off, int maxLen) 
{
  return readImpl(buff, length, off, maxLen);
}

void FileInStream::close()
{
  closeImpl();
}

long FileInStream::skip(long n)
{
  return skipImpl(n);
}

void FileInStream::seek(long pos)
{
  seekImpl(pos);
}

//////////////////////////////////////////
// EmptyBlockInStream
//////////////////////////////////////////

int EmptyBlockInStream::read()
{
  return readImpl();
}

int EmptyBlockInStream::read(void *buff, int length, int off, int maxLen) 
{
  return readImpl(buff, length, off, maxLen);
}

void EmptyBlockInStream::close()
{
  closeImpl();
}

long EmptyBlockInStream::skip(long n)
{
  return skipImpl(n);
}

void EmptyBlockInStream::seek(long pos)
{
  seekImpl(pos);
}

//////////////////////////////////////////
// LocalBlockInStream
//////////////////////////////////////////

int LocalBlockInStream::read()
{
  return readImpl();
}

int LocalBlockInStream::read(void *buff, int length, int off, int maxLen) 
{
  return readImpl(buff, length, off, maxLen);
}

void LocalBlockInStream::close()
{
  closeImpl();
}

long LocalBlockInStream::skip(long n)
{
  return skipImpl(n);
}

void LocalBlockInStream::seek(long pos)
{
  seekImpl(pos);
}

//////////////////////////////////////////
// RemoteBlockInStream
//////////////////////////////////////////

int RemoteBlockInStream::read()
{
  return readImpl();
}

int RemoteBlockInStream::read(void *buff, int length, int off, int maxLen) 
{
  return readImpl(buff, length, off, maxLen);
}

void RemoteBlockInStream::close()
{
  closeImpl();
}

long RemoteBlockInStream::skip(long n)
{
  return skipImpl(n);
}

void RemoteBlockInStream::seek(long pos)
{
  seekImpl(pos);
}

//////////////////////////////////////////
// OutStream
//////////////////////////////////////////

void OutStream::cancelImpl()
{
  m_env.callMethod(NULL, m_obj, TOSTREAM_CANCEL_METHD, "()V");
}

void OutStream::closeImpl()
{
  m_env.callMethod(NULL, m_obj, TOSTREAM_CLOSE_METHD, "()V");
}

void OutStream::flushImpl()
{
  m_env.callMethod(NULL, m_obj, TOSTREAM_FLUSH_METHD, "()V");
}

void OutStream::writeImpl(int byte) 
{
  m_env.callMethod(NULL, m_obj, TOSTREAM_WRITE_METHD, "(I)V", (jint) byte);
}

void OutStream::writeImpl(const void *buff, int length, 
                          int off, int maxLen)
{
  jthrowable exception;
  jbyteArray jBuf;

  jBuf = m_env.newByteArray(length);
  m_env->SetByteArrayRegion(jBuf, 0, length, (jbyte*) buff);

  char *jbuff = (char *) malloc(length * sizeof(char));
  m_env->GetByteArrayRegion(jBuf, 0, length, (jbyte*) jbuff);
  // printf("byte array in write: %s\n", jbuff);

  if (off < 0 || maxLen <= 0 || length == maxLen)
    m_env.callMethod(NULL, m_obj, TOSTREAM_WRITE_METHD,
                  "([B)V", jBuf);
  else
    m_env.callMethod(NULL, m_obj, TOSTREAM_WRITE_METHD,
                  "([BII)V", jBuf, (jint) off, (jint) maxLen);
  m_env->DeleteLocalRef(jBuf);
}

// Call the templates

void OutStream::cancel() 
{
  cancelImpl();
}

void OutStream::close()
{
  closeImpl();
}

void OutStream::flush()
{
  flushImpl();
}

void OutStream::write(const void *buff, int length, int off, int maxLen)
{
  writeImpl(buff, length, off, maxLen);
}

void OutStream::write(const void *buff, int length)
{
  write(buff, length, 0, length);
}

//////////////////////////////////////////
// FileOutStream
//////////////////////////////////////////

void FileOutStream::cancel() 
{
  cancelImpl();
}

void FileOutStream::close()
{
  closeImpl();
}

void FileOutStream::flush()
{
  flushImpl();
}

void FileOutStream::write(const void *buff, int length, int off, int maxLen)
{
  writeImpl(buff, length, off, maxLen);
}

//////////////////////////////////////////
// BlockOutStream
//////////////////////////////////////////

void BlockOutStream::cancel() 
{
  cancelImpl();
}

void BlockOutStream::close()
{
  closeImpl();
}

void BlockOutStream::flush()
{
  flushImpl();
}

void BlockOutStream::write(const void *buff, int length, int off, int maxLen)
{
  writeImpl(buff, length, off, maxLen);
}

//////////////////////////////////////////
// TachyonURI
//////////////////////////////////////////

jTachyonURI TachyonURI::newURI(const char *pathStr)
{
  Env env;
  jobject retObj;
  jstring jPathStr;
  
  jPathStr = env.newStringUTF(pathStr, "path");
  retObj = env.newObject(TURI_CLS, "(Ljava/lang/String;)V", jPathStr);
  env->DeleteLocalRef(jPathStr);
  return new TachyonURI(env, retObj);
}

jTachyonURI TachyonURI::newURI(const char *scheme, const char *authority, const char *path)
{
  Env env;
  jobject retObj;
  jstring jscheme, jauthority, jpath;

  jscheme = env.newStringUTF(scheme, "scheme");
  jauthority = env.newStringUTF(authority, "authority");
  jpath = env.newStringUTF(path, "path");
  retObj = env.newObject(TURI_CLS,
                  "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", 
                  jscheme, jauthority, jpath);
  env->DeleteLocalRef(jscheme);
  env->DeleteLocalRef(jauthority);
  env->DeleteLocalRef(jpath);
  return new TachyonURI(env, retObj);
}

jTachyonKV TachyonKV::createKV(jTachyonClient client, ReadType readType, 
        WriteType writeType, long blockSizeByte, const char *kvStore)
{
  Env& env = client->getEnv();
  jobject retObj, ereadtObj, ewritetObj;

  ereadtObj = enumObjReadType(env, readType);
  ewritetObj = enumObjWriteType(env, writeType);

  if (kvStore == NULL || strlen(kvStore) == 0) {
    retObj = env.newObject(TKV_CLS,
                  "(Ltachyon/client/TachyonFS;Ltachyon/client/ReadType;Ltachyon/client/WriteType;J)V", 
                  client->getJObj(), ereadtObj, ewritetObj, (jlong) blockSizeByte);
  } else {
    jstring jKVStr = env.newStringUTF(kvStore, "kvstore");
    retObj = env.newObject(TKV_CLS,
                  "(Ltachyon/client/TachyonFS;Ltachyon/client/ReadType;Ltachyon/client/WriteType;JLjava/lang/String;)V", 
                  client->getJObj(), ereadtObj, ewritetObj, (jlong) blockSizeByte, jKVStr);
    env->DeleteLocalRef(jKVStr);
  }
  if (retObj == NULL)
    return NULL;
  return new TachyonKV(client, retObj);
}

bool TachyonKV::init()
{
  jthrowable exception;
  jvalue ret;
  m_env.callMethod(&ret, m_obj, TKV_INIT_METHD, "()Z");
  return ret.i;
}

int TachyonKV::get(const char *key, uint32_t keylen, char *buff, uint32_t valuelen)
{
  int rdSz;
  jobject jDbuff;
  jvalue ret;

  m_env.callMethod(&ret, m_obj, TKV_GRBUFF_METHD,
          "()Ljava/nio/ByteBuffer;");
  jDbuff = ret.l;

  std::string skey(key, keylen);
  jstring jKeyStr = m_env.newStringUTF(skey.c_str(), "key");
  m_env.callMethod(&ret, m_obj, TKV_RBUFF_METHD,
          "(Ljava/lang/String;Ljava/nio/ByteBuffer;)I", jKeyStr, jDbuff);
  m_env->DeleteLocalRef(jKeyStr);
  char *cDbuff = (char *) m_env->GetDirectBufferAddress(jDbuff);
  if (cDbuff == NULL) {
    serror("fail to get direct buffer address");
    return -1;
  }
  rdSz = ret.i;
  strncpy(buff, cDbuff, valuelen);

  /*
  jthrowable exception;
  jbyteArray jBuf;
  jvalue ret;

  std::string skey(key, keylen);
  jstring jKeyStr = m_env->NewStringUTF(skey.c_str());
  if (jKeyStr == NULL) {
    serror("fail to allocate key string");
    return NULL;
  }

  jBuf = m_env->NewByteArray(valuelen);
  if (jBuf == NULL) {
    serror("fail to allocate jByteArray for TachyonKV.get");
    return -1;
  }
  exception = callMethod(m_env, &ret, m_obj, TKV_CLS, TKV_GET_METHD,
          "(Ljava/lang/String;[B)I", false, jKeyStr, jBuf);
  m_env->DeleteLocalRef(jKeyStr);
  if (exception != NULL) {
    m_env->DeleteLocalRef(jBuf);
    // serror("fail to call TachyonKV.get()");
    printException(m_env, exception);
    return -1;
  }
  rdSz = ret.i;
  if (rdSz > 0) {
    m_env->GetByteArrayRegion(jBuf, 0, valuelen, (jbyte*) buff);
  }
  m_env->DeleteLocalRef(jBuf);
  */
  return rdSz;
}

void TachyonKV::set(const char *key, uint32_t keylen, const char *buff, uint32_t valuelen)
{
  jobject jDbuff;

  jDbuff = m_env->NewDirectByteBuffer((void *) buff, valuelen);
  if (jDbuff == NULL) {
    serror("fail to create direct byte buffer");
    return;
  }

  std::string skey(key, keylen);
  jstring jKeyStr = m_env.newStringUTF(skey.c_str(), "key");
  m_env.callMethod(NULL, m_obj, TKV_WBUFF_METHD,
          "(Ljava/lang/String;Ljava/nio/ByteBuffer;)V", jKeyStr, jDbuff);
  m_env->DeleteLocalRef(jKeyStr);

  /*
  jthrowable exception;
  jbyteArray jBuf;

  std::string skey(key, keylen);
  jstring jKeyStr = m_env->NewStringUTF(skey.c_str());
  if (jKeyStr == NULL) {
    serror("fail to allocate key string");
    return;
  }

  jBuf = m_env->NewByteArray(valuelen);
  if (jBuf == NULL) {
    serror("fail to allocate jByteArray for TachyonKV.set");
    return;
  }
  m_env->SetByteArrayRegion(jBuf, 0, valuelen, (jbyte*) buff);

  exception = callMethod(m_env, NULL, m_obj, TKV_CLS, TKV_SET_METHD,
          "(Ljava/lang/String;[B)V", false, jKeyStr, jBuf);
  m_env->DeleteLocalRef(jKeyStr);
  if (exception != NULL) {
    m_env->DeleteLocalRef(jBuf);
    // serror("fail to call TachyonKV.set()");
    printException(m_env, exception);
  }
  */
}

jobject enumObjReadType(Env& env, ReadType readType)
{
  const char *valueName;
  switch (readType) {
    case NO_CACHE: 
          valueName = "NO_CACHE";
          break;
    case CACHE:
          valueName = "CACHE";
          break;
    case CACHE_PROMOTE:
          valueName = "CACHE_PROMOTE";
          break;
    default:
          throw std::runtime_error("invalid readType");
  }
  return env.getEnumObject(TREADT_CLS, valueName);
}

jobject enumObjWriteType(Env& env, WriteType writeType)
{
  const char *valueName;
  switch (writeType) {
    case ASYNC_THROUGH: 
          valueName = "ASYNC_THROUGH";
          break;
    case CACHE_THROUGH:
          valueName = "CACHE_THROUGH";
          break;
    case MUST_CACHE:
          valueName = "MUST_CACHE";
          break;
    case THROUGH:
          valueName = "THROUGH";
          break;
    case TRY_CACHE:
          valueName = "TRY_CACHE";
          break;
    default:
          throw std::runtime_error("invalid writeType");
  }
  return env.getEnumObject(TWRITET_CLS, valueName);
}

char* fullTachyonPath(const char *masterUri, const char *filePath)
{
  size_t mlen, flen, nlen;
  bool slash;
  char *fullPath;

  mlen = strlen(masterUri);
  flen = strlen(filePath);
  if (mlen == 0 || flen == 0) {
    return NULL;
  }
  if (flen >= mlen) {
    if (strncmp(masterUri, filePath, mlen) == 0) {
      // it's already a full path
      fullPath = (char *) malloc((flen + 1) * sizeof(char));
      if (fullPath != NULL) {
        strncpy(fullPath, filePath, flen);
        fullPath[flen] = '\0';
      }
      return fullPath;
    }
  }
  slash = (masterUri[mlen - 1] == '/' || filePath[flen - 1] != '/');
  if (slash)
    nlen = mlen + flen + 1;
  else
    nlen = mlen + flen + 2; // need to insert a slash
  fullPath = (char *) malloc(nlen * sizeof(char));
  if (fullPath != NULL) {
    if (slash)
      snprintf(fullPath, nlen, "%s%s", masterUri, filePath);
    else
      snprintf(fullPath, nlen, "%s/%s", masterUri, filePath);
  }
  return fullPath;
}

/* vim: set ts=4 sw=4 : */
