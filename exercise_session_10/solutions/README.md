Exercise 01

  * Turn on the UZH VPN and login to Science Cloud
  * Create a new instance using the image called `hadoop`
  * Choose an instance flavor and use/create/import your key
  * Copy the setup from the exercise sheet into your `$HOME/.ssh/config` file, change the `Hostname` accordingly (see IP-address of your new instance)
  * Once you log into your Hadoop instance, you should be able to execute scripts called `start-dfs.sh` and `start-yarn.sh` from your `$HOME` directory

Exercise 02

  * In the first part, let's run map & reduce on Eiger instead of distributed file system. Log into Eiger and enter the cloned `exercise_session_10` directory. Then execure the following commands, piped together:
  ```bash
     cat trump_tweets_0* | ./mapper.py | sort | ./reducer.py > output.log
  ```
 We will find the output of map & reduce in `output.log` file.

 * Now, let's execute the  same, but now using Hadoop DFS. Once logged in the hadoop instance, let's create two new directories inside the hadoop filesystem, named in an agreement with `-input` and `-output` flags of the `pymapred.sh` script.
   ```bash
      hadoop fs -mkdir DonaldTrump # here, the input files will be stored
      hadoop fs -mkdir output      # here, the input files will be stored
   ```
* Next, let's copy the tweet files to the input folder of hadoop filesystem.
   ```bash
      hadoop fs -put trump_tweets_*.csv DonaldTrump/
   ```
* Now, we are ready to launch the map&reduce on hadoop:
   ```bash
      ./pymapred.sh
   ```
* Two files were created in the output folder on hadoop, namely `_SUCCESS` (empty) and `part-00000` containing the final reduced data. One can copy the output file from hadoop fs back to local directory using
   ```bash
      hadoop fs -get ./output/part-00000 .
   ```

We attach the output file (`part-00000`) and the log file from the map&reduce run (`pymapred.log`). Some informative lines from the log file are the following:

```text
Launched map tasks=7
Launched reduce tasks=1

Map input records=35371
Map output records=538568

Reduce input records=538568
Reduce output records=47090
```

Especially, the last row of the above is number of items in the `part-00000` file. 

Web API can be accessed under `http://localhost:9870`. The output files can be found by navigating to `utilities -> Browse the file system -> user -> ubuntu -> output`
