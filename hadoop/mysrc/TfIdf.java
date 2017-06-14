package edu.umich.cse.eecs485;

import java.io.IOException;
import java.util.StringTokenizer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import java.util.HashMap;
import java.util.HashSet;
import java.util.ArrayList;
import java.util.Arrays;
import nu.xom.*;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TfIdf
{
	public static int n = 0;
	public static class Map extends Mapper<LongWritable, Text, Text, Text> {

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			// sections[0] = word, sections[1] = docID, section[2] = tf  sections[3] = docId sections[4]= tf sections[5] = :nk |||||||(could be more) sections[3] = docID
			//sections [size-1] = NF (Ignore)
			String tempValue = value.toString();
			String [] sections = tempValue.split(" ");
			// if (count == 0) {
			// 	context.write(new Text("DocOccurences, "), new Text("hello"));
			// 	//context.write(new Text("TotalDocs , "), new Text(docs));
			// 	++count;	
			// } else {
			int i = 1;
			while (i < (sections.length - 1)) {

				if (sections.length == 3) {
					String docCount = sections[1].replaceAll("\\s+","");
					n = Integer.parseInt(docCount);
				} else {
					String output = sections[i] + "," + sections[i + 1] + "," + sections[sections.length - 1];
					String _key = sections[0].replaceAll("\\s+", "");
					context.write(new Text(_key), new Text(output));
				}
				i = i + 2;
			}
			//}
			
		}
	}



	public static class Reduce extends Reducer<Text, Text, Text, Text> {
		public static class DocInfo {
			double idf;
			double tfidf;
			int nk;
			int tf;
			int docId;
		}

		public void reduce(Text key, Iterable<Text> values, Context context)
				throws IOException, InterruptedException {
			HashMap<String, ArrayList<DocInfo>> finalMap = new HashMap<String, ArrayList<DocInfo>>();

			String skey = key.toString();

			for (Text value : values) {
				String line = value.toString();
				String [] sections = line.split(",");


				DocInfo temp = new DocInfo();
				temp.idf = 0.0;
				String s1 = sections[0].replaceAll("\\s+","");
				String s2 = sections[1].replaceAll("\\s+","");
				String s3 = sections[2].replaceAll("\\s+","");

				temp.docId = Integer.parseInt(s1);
				temp.tf = Integer.parseInt(s2);
				temp.nk = Integer.parseInt(s3);


				ArrayList<DocInfo> update = new ArrayList<DocInfo>();
				if(finalMap.containsKey(skey)){ //Just want to insert in vector that is associated with Word
					update = finalMap.get(skey);
					update.add(temp);
					finalMap.put(skey, update);
				} else{
					update.add(temp);
					finalMap.put(skey, update);
				}
			}

			ArrayList<DocInfo> oneWord = new ArrayList<DocInfo>();
			oneWord = finalMap.get(skey);
			int _nk = oneWord.get(0).nk; 
			String output = "," + Integer.toString(_nk);
			for (int i = 0; i < oneWord.size(); ++i) {
				oneWord.get(i).idf = Math.log10((double)n / _nk);
				oneWord.get(i).tfidf = oneWord.get(i).tf * oneWord.get(i).idf;
				output += "," + oneWord.get(i).docId + "," + oneWord.get(i).tfidf;
			}


			context.write(new Text(key), new Text(output));
		}
	}

	public static void main(String[] args) throws Exception
	{

		Configuration conf = new Configuration();

		Job job = new Job(conf, "nk");

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		job.setMapperClass(Map.class);
		job.setReducerClass(Reduce.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.waitForCompletion(true);
	}
}