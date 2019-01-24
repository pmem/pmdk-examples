#
# Copyright (c) 2018-2019, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Intel Corporation nor the names of its contributors
#       may be used to endorse or promote products derived from this software
#       without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

PROGS = normal leaker checker fixer
OBJS = normal.o leaker.o checker.o fixer.o
CXXFLAGS = $(shell pkg-config --cflags libpmemobj++) -std=c++11 -O2
LDFLAGS = $(shell pkg-config --libs libpmemobj++) -O2
CXX = g++
RM = rm

all: $(PROGS)
.PHONY : all

%.o : %.cpp common.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

normal: normal.o
	$(CXX) $< $(LDFLAGS) -o $@
leaker: leaker.o
	$(CXX) $< $(LDFLAGS) -o $@
checker: checker.o
	$(CXX) $< $(LDFLAGS) -o $@
fixer: fixer.o
	$(CXX) $< $(LDFLAGS) -o $@

clean:
	$(RM) $(OBJS) $(PROGS)

