package is.hi.hpc;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeMap;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FileUtil;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class Main {
	/**
	 * Threshold for values to be considered relevant. Any TF-IDF below this will
	 * not get emitted during csv generation.
	 */
	public static final double THRESHOLD = .00001;
	
	private static String inputPath;
	private static String outputPath;
	private static String tmp;
	
	private static boolean job1(Configuration conf) throws Exception {
	    Job job = new Job(conf, "gutenberg-preprocessor");
	    
	    job.setJarByClass(Main.class);
	    
	    job.setOutputKeyClass(Text.class);
	    job.setOutputValueClass(IntWritable.class);
	        
	    //job.setMapperClass(WordCount.Map.class);
	    //job.setReducerClass(WordCount.Reduce.class);
	        
	    //FileInputFormat.addInputPath(job, new Path(args[0]));
	    //FileOutputFormat.setOutputPath(job, new Path(args[1]));
	        
	    ///ZipFileInputFormat.setLenient( true );
	    //ZipFileInputFormat.setInputPaths(job, new Path(inputPath));
	    //TextOutputFormat.setOutputPath(job, new Path(tmp));
	    
	    return job.waitForCompletion(true);
	}
	
	public static void main(String[] args) throws Exception {
	    Configuration conf = new Configuration();
	    inputPath = "gbp-inputs/*.zip";
	    outputPath = "gbp-outputs";
	    tmp = "gbp-tmp";
	    
	    //Need to hard code number of documents
	    conf.set("corpus", args[0]);
	    
	    job1(conf);
	 }
}
