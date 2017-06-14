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
import java.util.LinkedList;
import nu.xom.*;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class normalize
{
	public static int n = 0;
	public static class Normalize_Map extends Mapper<LongWritable, Text, Text, Text> {

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			String tempValue = value.toString();
			String [] sections = tempValue.split(",");

			int i = 1;
			while (i < (sections.length - 1)) {
				String output = sections[0] + "," + sections[i + 2] + "," + sections[1];
				context.write(new Text(sections[i + 1]), new Text(output));
				i = i + 2;
			}			
		}
	}



	public static class Normalize_Reduce extends Reducer<Text, Text, Text, Text> {
		public void reduce(Text key, Iterable<Text> values, Context context)
				throws IOException, InterruptedException {
			HashMap<String, Integer> finalMap = new HashMap<String, Integer>();

			String skey = key.toString();
			
			double normal = 0.0;
			LinkedList<String> valuesList = new LinkedList<String>();
			for (Text value : values) {
				String line = value.toString();
				String [] sections = line.split(",");
				String s1 = sections[0].replaceAll("\\s+",""); //Word
				String s2 = sections[1].replaceAll("\\s+",""); //Tfidf
				double _tfidf = Double.parseDouble(s2);
				String s3 = sections[2].replaceAll("\\s+",""); //Nk
				normal += _tfidf;
				valuesList.add(value.toString());
			}

			normal = Math.sqrt(normal);
			Iterator<String> ite = valuesList.iterator();
			while (ite.hasNext()) {
				String line = ite.next();
				String [] sections = line.split(",");

				String s1 = sections[0].replaceAll("\\s+",""); //Word
				String s2 = sections[1].replaceAll("\\s+",""); //Tfidf
				double _tfidf = Double.parseDouble(s2);
				String s3 = sections[2].replaceAll("\\s+",""); //Nk
				String output = "," + s3 + "," + skey + "," + _tfidf / normal;


				context.write(new Text(s1), new Text(output));
			}
		}
	}

	public static void main(String[] args) throws Exception
	{

		Configuration conf = new Configuration();

		Job job = new Job(conf, "nk");

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		job.setMapperClass(Normalize_Map.class);
		job.setReducerClass(Normalize_Reduce.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.waitForCompletion(true);
	}
}