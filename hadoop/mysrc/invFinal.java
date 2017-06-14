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
import java.text.*;

public class invFinal
{
	public static int n = 0;
	public static class invFinal_Map extends Mapper<LongWritable, Text, Text, Text> {

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			String tempValue = value.toString();
			String [] sections = tempValue.split(",");

			String output = sections[2] + "," + sections[3] + "," + sections[1];
			context.write(new Text(sections[0]), new Text(output));
		}
	}



	public static class invFinal_Reduce extends Reducer<Text, Text, Text, Text> {
		public void reduce(Text key, Iterable<Text> values, Context context)
				throws IOException, InterruptedException {
			HashMap<String, Integer> finalMap = new HashMap<String, Integer>();

			String skey = key.toString(); //Word


			String output = "";
			int count  = 0;
			for (Text value : values) {
				String line = value.toString();
				String [] sections = line.split(",");
				if (count == 0) {
					output = sections[2].replaceAll("\\s+","");
					++count;
				}
				String tfidf = sections[1].replaceAll("\\s+",""); 
				String docId = sections[0].replaceAll("\\s+","");
				
				NumberFormat formatter = new DecimalFormat();
				Double toBeTruncated = new Double(tfidf);
				
				formatter = new DecimalFormat("0.####E0");
				tfidf = formatter.format(toBeTruncated);

				output += " " + docId + ":" + tfidf;
			}

			context.write(new Text(skey), new Text(output));
		}
	}

	public static void main(String[] args) throws Exception
	{

		Configuration conf = new Configuration();

		Job job = new Job(conf, "nk");

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		job.setMapperClass(invFinal_Map.class);
		job.setReducerClass(invFinal_Reduce.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.waitForCompletion(true);
	}
}