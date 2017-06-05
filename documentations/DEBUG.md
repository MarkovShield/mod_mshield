# Debug mod_mshield

## Content
<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:0 orderedList:0 -->

- [Debug mod_mshield](#markdown-header-debug-modmshield)
	- [Content](#markdown-header-content)
	- [Debugging & helpful commands](#markdown-header-debugging-helpful-commands)
		- [Install usefull debugging tools](#markdown-header-install-usefull-debugging-tools)
		- [Apache logs](#markdown-header-apache-logs)
		- [Trace performance](#markdown-header-trace-performance)
		- [Debug segmentation fault](#markdown-header-debug-segmentation-fault)
		- [Kafka](#markdown-header-kafka)
			- [Test kafka connection](#markdown-header-test-kafka-connection)
			- [Send test message to Kafka](#markdown-header-send-test-message-to-kafka)
			- [Show consumer groups](#markdown-header-show-consumer-groups)
		- [Redis](#markdown-header-redis)
			- [Listen on multiple channels](#markdown-header-listen-on-multiple-channels)
			- [Test Redis connection](#markdown-header-test-redis-connection)
	- [mod_mshield debugging container](#markdown-header-mod_mshield-debugging-container)

<!-- /TOC -->

## Debugging & helpful commands

### Install usefull debugging tools
To debug mod_mshield the following tools are a good collection to start the challenge. Make sure you have installed them on your system if you follow these instructions.
```bash
apt-get update
apt-get install -y gdb valgrind apache2-dbg libapr1-dbg libaprutil1-dbg binutils linux-tools
```

### Apache logs
View the mod_mshield logs you could use `tail -f`. Change the log file path depending on your setup.
```bash
tail -f /opt/applic/httpd/logs/*error_log
```

### Trace performance
To trace the performance of `mod_mshield` or `apache` in general, the tool `perf` does a quite good job.

Start the apache server with the `-X` option to run it in debugging mode where only a single worker is started.
```bash
/usr/sbin/apachectl -DFOREGROUND -k start -e debug -X
```

In another terminal start `perf` using the options shown or add/replace some options depending on your needs.
```bash
perf record -F 100 -p <PID>
```

Another possibility is to run `perf` so it traces the function call graphs:
```bash
perf record -a --call-graph dwarf -p <PID-of-apache2-process>
```

If `perf` failed because of kernel restrictions, allow is by issuing the following command:
```bash
echo 0 > /proc/sys/kernel/kptr_restrict
```

Now you can access the website the manual way or run the load tests provided by MarkovShield. To run the load tests have a look at the `markovshield_benchmark` repository.

To view the `perf` results hit `perf report` to use the interactive reporting tool from `perf`.

### Debug segmentation fault
To debug a segmentation fault follow the steps shown here to have a good chance to detect the faulty spot in the code:
```bash
# Source the apache environment variables
source /etc/apache2/envvars
# Run gdb with the command apache2
gdb apache2
# You should now be inside the gdb console.
# Set a break point to the function in which you think the segmentation fault could be triggered
b mshield_access_checker
# Now run the apache2 command with the -X option to run it with a single worker.
run -X
```

### Kafka

#### Test kafka connection
Listen on topic (e.g. `MarkovClicks`):
```bash
kafkacat -C -b 192.168.56.50 -t MarkovClicks
```

#### Send test message to Kafka
```bash
echo $'xtTALCofbVIMEmuJzd95Me0prdFNKt%{"sessionUUID":	"xtTALCofbVIMEmuJzd95Me0prdFNKt","clickUUID":	"zugbwerz23g8gzbhb","timeStamp":	1493639064719,"url":	"/private/request-header/","urlRiskLevel":	4,"validationRequired":	true}' | kafka-console-producer.sh \
    --broker-list localhost:9092 \
    --topic MarkovClicks \
    --property parse.key=true \
    --property key.separator=%
```

#### Show consumer groups
```bash
kafka-consumer-groups.sh --list --bootstrap-server localhost:9092
```

### Redis

#### Listen on multiple channels
```bash
redis-cli psubscribe W*
```

#### Test Redis connection
Publish engine result (e.g. `FRAUD`) to channel `zugbwerz23g8gzbhb` :
```bash
redis-cli publish zugbwerz23g8gzbhb FRAUD
```
**Hint:** The `clickUUID` is used as channel ID.

## mod_mshield debugging container
If you want to debug `mod_mshield` inside a docker container have a look at the files in the `dev-docker-image` directory.

First you need the MarkovShield demo application running in the background. Please follow the steps from [Getting started](GETTING_STARTED.md) to achive this. In additon to that you need to stop the existing `mshielddemo_mshield_reverse_proxy_1` container by hitting the command `docker stop mshielddemo_mshield_reverse_proxy_1`.

To create the `mod_mshield` docker container follow the steps shown down here:

1.  Backup the regular `Dockerfile` inside `examplesite/reverseproxy` be renaming it to something like `Dockerfile_orig`.
2.  Copy all files from `dev-docker-image` to `examplesite/reverseproxy`.
3.  Build the docker debug container using the `build_it.sh` script.
4.  Finally run the `mod_mshield` debug container inside the existing `make demo` docker-compose network by running `run_it.sh`.
