#!/bin/bash

#PBS -q batch
#PBS -N gutenberg-preprocessor
#PBS -l nodes=2:ppn=4
#PBS -o hadoop_run.out
#PBS -e hadoop_run.err
#PBS -V

module add java

### Run the myHadoop environment script to set the appropriate variables
#
# Note: ensure that the variables are set correctly in bin/setenv.sh
. /N/soft/myHadoop/bin/setenv.sh

#### Set this to the directory where Hadoop configs should be generated
# Don't change the name of this variable (HADOOP_CONF_DIR) as it is
# required by Hadoop - all config files will be picked up from here
#
# Make sure that this is accessible to all nodes
export HADOOP_CONF_DIR="${HOME}/elevato-config"

#### Set up the configuration
# Make sure number of nodes is the same as what you have requested from PBS
# usage: $MY_HADOOP_HOME/bin/pbs-configure.sh -h
echo "Set up the configurations for myHadoop"
# this is the non-persistent mode
$MY_HADOOP_HOME/bin/pbs-configure.sh -n 4 -c $HADOOP_CONF_DIR
# this is the persistent mode
# $MY_HADOOP_HOME/bin/pbs-configure.sh -n 4 -c $HADOOP_CONF_DIR -p -d /oasis/cloudstor-group/HDFS
echo

#### Format HDFS, if this is the first time or not a persistent instance
echo "Format HDFS"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR namenode -format
echo

#### Start the Hadoop cluster
echo "Start all Hadoop daemons"
$HADOOP_HOME/bin/start-all.sh
#$HADOOP_HOME/bin/hadoop dfsadmin -safemode leave
echo

#### Run your jobs here
echo "Cooking potatoes"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -mkdir gbp-inputs
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -copyFromLocal ~/dist/*.txt elevato-inputs
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -ls gbp-inputs
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR jar ~/dist/hpc-elevato-potato.jar
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -ls gbp-outputs
#$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -copyToLocal gbp-outputs ${HOME}/gutenberg-preprocessor-outputs
echo

#### Stop the Hadoop cluster
echo "Stop all Hadoop daemons"
$HADOOP_HOME/bin/stop-all.sh
echo

#### Clean up the working directories after job completion
echo "Clean up"
$MY_HADOOP_HOME/bin/pbs-cleanup.sh -n 4 -c $HADOOP_CONF_DIR
echo
