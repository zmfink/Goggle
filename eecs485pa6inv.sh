#!/bin/bash

cd hadoop
cd mysrc


rm -rf *.jar bin/* output tf_output tf_nk_output tfidf_output normalize_output ../../invoutput

javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin TFik.java XmlInputFormat.java

jar -cvf ./TFik.jar -C bin/ .

javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin Nk.java XmlInputFormat.java
jar -cvf ./Nk.jar -C bin/ .

javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin TfIdf.java XmlInputFormat.java
jar -cvf ./TfIdf.jar -C bin/ .

javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin normalize.java XmlInputFormat.java
jar -cvf ./normalize.jar -C bin/ .

javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin invFinal.java XmlInputFormat.java
jar -cvf ./invFinal.jar -C bin/ .

export JAVA_HOME=/usr

..//bin/hadoop jar TFik.jar edu.umich.cse.eecs485.TFik ../dataset/mining.articles.xml tf_output
..//bin/hadoop jar Nk.jar edu.umich.cse.eecs485.Nk tf_output tf_nk_output
..//bin/hadoop jar TfIdf.jar edu.umich.cse.eecs485.TfIdf tf_nk_output tfidf_output
..//bin/hadoop jar normalize.jar edu.umich.cse.eecs485.normalize tfidf_output normalize_output
..//bin/hadoop jar invFinal.jar edu.umich.cse.eecs485.invFinal normalize_output ../../invoutput