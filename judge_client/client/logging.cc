/*
 * Copyright 2007 Xu, Chuan <xuchuan@gmail.com>
 *
 * This file is part of ZOJ.
 *
 * ZOJ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZOJ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZOJ. if not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/socket.h>
#include <sys/un.h>

#include "logging.h"
#include "args.h"
#include "util.h"

LogFile* Log::log_ = NULL;
bool Log::log_to_stderr_ = true;

Log::Log(const char* filename, int line_number, int level) {
    static const char* LEVEL_NAME[] =
            {"ERROR", "DEBUG", "WARNING", "ERROR", "FATAL", "INFO"};
    if ((log_ || log_to_stderr_) && level != RAW) {
        message_stream_<<GetLocalTimeAsString("%Y-%m-%d %H:%M:%S")<<' '
                       <<filename<<':'<<line_number<<' '<<LEVEL_NAME[level]<<' ';
        if (level == SYSCALL_ERROR) {
            message_stream_<<strerror(errno)<<". ";
        }
    }
}

Log::~Log() {
    string message;
    if (log_ || log_to_stderr_) {
        message_stream_<<endl;
        message = message_stream_.str();
    }
    if (log_) {
        log_->Write(message);
    }
    if (log_to_stderr_) {
        fprintf(stderr, "%s", message.c_str());
    }
}

LogFile::~LogFile() {
}

void DiskLogFile::Write(const string& message) {
    if (fd_ < 0) {
        this->CreateNewFile();
    }
    if (fd_ >= 0) {
        if (size_ + message.size() > MAX_LOG_FILE_SIZE) {
            close(fd_);
            this->CreateNewFile();
        }
        if (fd_ >= 0) {
            size_ += message.size();
            Writen(fd_, message.c_str(), message.size());
        }
    }
}

void DiskLogFile::CreateNewFile() {
    fd_ = -1;
    size_ = 0;
    fd_ = open(filename_.c_str(), O_RDWR | O_CREAT | O_TRUNC);
    if (fd_ < 0) {
        string error_message = strerror(errno);
        openlog("ZOJ Judge Client", 0, LOG_USER);
        syslog(LOG_ERR, "Fail to create file %s: %s",
               filename_.c_str(), error_message.c_str());
    }
}

DiskLogFile::~DiskLogFile() {
    Close();
}

void DiskLogFile::Close() {
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

UnixDomainSocketLogFile::~UnixDomainSocketLogFile() {
    Close();
}

void UnixDomainSocketLogFile::Close() {
    if (sock_ >= 0) {
        close(sock_);
        sock_ = -1;
    }
}

void UnixDomainSocketLogFile::Write(const string& message) {
    if (sock_ < 0) {
        Connect();
    }
    if (sock_ >= 0) {
        Writen(sock_, message.c_str(), message.size());
    }
}

void UnixDomainSocketLogFile::Connect() {
    sock_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_ < 0) {
        string error_message = strerror(errno);
        openlog("ZOJ Judge Client", 0, LOG_USER);
        syslog(LOG_ERR, "Fail to create socket: %s", error_message.c_str());
        return;
    }
    struct sockaddr_un un;
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX; 
    string client_sock_name = "log.sock";
    unlink(client_sock_name.c_str());
    strcpy(un.sun_path, client_sock_name.c_str());
    if (bind(sock_, (struct sockaddr*)&un,
             (int)&((struct sockaddr_un*)0)->sun_path +
                    client_sock_name.size()) < 0) {
        string error_message = strerror(errno);
        openlog("ZOJ Judge Client", 0, LOG_USER);
        syslog(LOG_ERR, "Fail to bind: %s", error_message.c_str());
        return;
    }
    if (chmod(un.sun_path, S_IRWXU) < 0) {
        string error_message = strerror(errno);
        openlog("ZOJ Judge Client", 0, LOG_USER);
        syslog(LOG_ERR, "Fail to chmod: %s", error_message.c_str());
        return;
    }
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX; 
    string server_sock_name = root_ + "/log.sock";
    strcpy(un.sun_path, server_sock_name.c_str());
    if (connect(sock_, (struct sockaddr*)&un,
                (int)&((struct sockaddr_un*)0)->sun_path +
                        server_sock_name.size()) < 0) {
        string error_message = strerror(errno);
        openlog("ZOJ Judge Client", 0, LOG_USER);
        syslog(LOG_ERR, "Fail to connect: %s", error_message.c_str());
        return;
    }
}
