# Copyright 2010 Jason Stredwick
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CC =	g++
LD =	g++
CODE_DIR = Code

OBJECTS =	\
	Objs/TreeNode.o \
	Objs/GammaFunctions.o \
  Objs/NoncumulativeStem.o \
  Objs/Balance.o \
	Objs/NewickOutput.o \
	Objs/Utilities.o \
	Objs/Tree.o \
	Objs/TreeIterator.o \
	Objs/Avida.o \
  Objs/SimpleOrganism.o \
	Objs/random.o \
	Objs/ProgramInterface.o \
	Objs/main.o

all: Bin/TreeLoader

Bin/TreeLoader:	$(OBJECTS)
	$(LD) -o $@ $(OBJECTS)

Objs/random.o:	$(CODE_DIR)/Support/Interface/random.h \
		$(CODE_DIR)/Support/Source/random.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/Support/Source/random.cpp

Objs/TreeNode.o: 	$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Include/TreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Source/TreeNode.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/TreeNode.cpp

Objs/GammaFunctions.o:	$(CODE_DIR)/Support/Interface/OutputStream.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/GammaFunctions.h \
			$(CODE_DIR)/PhylogeneticTree/Source/GammaFunctions.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/GammaFunctions.cpp

Objs/NoncumulativeStem.o:	$(CODE_DIR)/Support/Interface/OutputStream.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/NoncumulativeStem.h \
			$(CODE_DIR)/PhylogeneticTree/Source/NoncumulativeStem.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/NoncumulativeStem.cpp

Objs/Balance.o:	$(CODE_DIR)/Support/Interface/OutputStream.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Balance.h \
			$(CODE_DIR)/PhylogeneticTree/Source/Balance.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/Balance.cpp

Objs/NewickOutput.o:	$(CODE_DIR)/Support/Interface/OutputStream.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/NewickOutput.h \
			$(CODE_DIR)/PhylogeneticTree/Source/NewickOutput.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/NewickOutput.cpp

Objs/Utilities.o:	$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/Support/Interface/random.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Utilities.h \
			$(CODE_DIR)/PhylogeneticTree/Source/Utilities.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/Utilities.cpp

Objs/Tree.o:		$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/PhylogeneticTree/Include/TreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/PhylogeneticTree/Source/Tree.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/Tree.cpp

Objs/TreeIterator.o:	$(CODE_DIR)/PhylogeneticTree/Include/TreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/TreeIterator.h \
			$(CODE_DIR)/PhylogeneticTree/Source/TreeIterator.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/PhylogeneticTree/Source/TreeIterator.cpp

Objs/Avida.o:		$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/Organisms/Interface/Avida.h \
			$(CODE_DIR)/Organisms/Source/Avida.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/Organisms/Source/Avida.cpp

Objs/SimpleOrganism.o:		$(CODE_DIR)/PhylogeneticTree/Interface/iOrganism.h \
			$(CODE_DIR)/Organisms/Interface/SimpleOrganism.h \
			$(CODE_DIR)/Organisms/Source/SimpleOrganism.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/Organisms/Source/SimpleOrganism.cpp

Objs/ProgramInterface.o:	$(CODE_DIR)/Support/Interface/random.h \
			$(CODE_DIR)/Support/Interface/OutputStream.h \
			$(CODE_DIR)/Organisms/Interface/Avida.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/GammaFunctions.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Utilities.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/iTreeNode.h \
			$(CODE_DIR)/PhylogeneticTree/Interface/Tree.h \
			$(CODE_DIR)/ProgramInterface.h \
			$(CODE_DIR)/ProgramInterface.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/ProgramInterface.cpp

Objs/main.o:		$(CODE_DIR)/ProgramInterface.h \
			$(CODE_DIR)/main.cpp
	$(CC) -I $(CODE_DIR) -o $@ -c $(CODE_DIR)/main.cpp

clean:
	rm $(OBJECTS); rm Bin/TreeLoader; find . -name '*~' -exec rm -f {} \;
