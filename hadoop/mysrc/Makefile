all: wordcount TFik Nk TfIdf normalize invFinal

wordcount:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin WordCount.java 
	jar -cvf ./WordCount.jar -C bin/ .

TFik:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin TFik.java XmlInputFormat.java
	jar -cvf ./TFik.jar -C bin/ .

Nk:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin Nk.java XmlInputFormat.java
	jar -cvf ./Nk.jar -C bin/ .

TfIdf:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin TfIdf.java XmlInputFormat.java
	jar -cvf ./TfIdf.jar -C bin/ .

normalize:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin normalize.java XmlInputFormat.java
	jar -cvf ./normalize.jar -C bin/ .

invFinal:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin invFinal.java XmlInputFormat.java
	jar -cvf ./invFinal.jar -C bin/ .

xom:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin XOMtest.java
	java -Xms1024m -Xmx1024m -cp .:../lib/*:bin/ edu.umich.cse.eecs485.XOMtest

clean:
	rm -rf *.jar bin/* output tf_output tf_nk_output tfidf_output normalize_output invFinal_output
