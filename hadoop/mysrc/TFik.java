package edu.umich.cse.eecs485;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.mahout.classifier.bayes.XmlInputFormat;
import java.util.HashSet;
import nu.xom.*;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.*;
import java.io.FileReader;
import java.io.BufferedReader;

public class TFik
{
	public static int numDocuments = 0;

	public static class Map_term extends Mapper<LongWritable, Text, Text, LongWritable> {
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			String strId = "";
			String strBody = "";

			// Parse the xml and read data (page id and article body)
			// Using XOM library
			Builder builder = new Builder();

			try {
				++numDocuments;
//				System.out.println(value.toString());
				Document doc = builder.build(value.toString(), null);

				Nodes nodeId = doc.query("//eecs485_article_id");
				strId = nodeId.get(0).getChild(0).getValue();
				
				Nodes nodeBody = doc.query("//eecs485_article_body");
				strBody = nodeBody.get(0).getChild(0).getValue();
			}
			// indicates a well-formedness error
			catch (ParsingException ex) { 
				System.out.println("Not well-formed.");
				System.out.println(ex.getMessage());
			}  
			catch (IOException ex) {
				System.out.println("io exception");
			}
			
			// Tokenize document body
			Pattern pattern = Pattern.compile("\\w+");
			Matcher matcher = pattern.matcher(strBody);
			
			while (matcher.find()) {
				// Write the parsed token
				String tempWord = matcher.group().toLowerCase();
				String lowerWord = tempWord.replaceAll("[^A-Za-z0-9]", "");
				String lowerWordF = lowerWord.replaceAll("\\s+", "");
				if (!lowerWordF.equals("")) {
					// System.out.println(lowerWord);
					context.write(new Text(lowerWordF), new LongWritable(Integer.valueOf(strId)));
				}
			}
		}
	}

	public static class Reduce_term extends Reducer<Text, LongWritable, Text, Text> {
		private static int count = 0;
		public void reduce(Text key, Iterable<LongWritable> values, Context context)
				throws IOException, InterruptedException {

			HashMap<String, Integer> stopWords = new HashMap<String, Integer>();
			try {
			    BufferedReader reader = new BufferedReader(new FileReader("stop.txt"));
			    String sWord;
			    while ((sWord = reader.readLine()) != null) {
			    	stopWords.put(sWord, 1);
			    }
			    reader.close();
			}
			catch (IOException ex) {
				System.out.println("io exception");
			}

			String word = key.toString();

			if (!(word.equals("")) && !stopWords.containsKey(word)) { //check if not a stop word
				
				HashMap<String, Integer> hm = new HashMap<String, Integer>();
		
				for (LongWritable value : values) {
					String pagid = String.valueOf(value.get());
			
					if(hm.containsKey(pagid)){
						Integer increase = hm.get(pagid);
						++increase;
						hm.put(pagid, increase);
					} else{
						hm.put(pagid, new Integer(1));
					}
				}
				// Convert the contents of the set into a list
				String invList = "";

				Iterator itr = hm.entrySet().iterator();
				while (itr.hasNext()) {
					Map.Entry pair = (Map.Entry<String, Integer>)itr.next();
					invList += " " + pair.getKey() + " " + pair.getValue();
					itr.remove();
				}

				if (count == 0) {
					String docs = " " + numDocuments;
					context.write(new Text("TotalDocs"), new Text(docs));
					++count;
				}
				context.write(key, new Text(invList));
			}
		}
	}

	public static void main(String[] args) throws Exception
	{
		Configuration conf = new Configuration();

		conf.set("xmlinput.start", "<eecs485_article>");
		conf.set("xmlinput.end", "</eecs485_article>");

		Job job = new Job(conf, "XmlParser");

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(LongWritable.class);

		job.setMapperClass(Map_term.class);
		job.setReducerClass(Reduce_term.class);

		job.setInputFormatClass(XmlInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.waitForCompletion(true);
	}
}
