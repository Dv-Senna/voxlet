import sys
import subprocess
import xml.etree.ElementTree as ET
import os
from dataclasses import dataclass, field
from pprint import pprint
import matplotlib.pyplot as plt
import numpy as np
import re

@dataclass
class BenchmarkRun:
    name: str = "",
    tag: str = "",
    variable_name: str = "",
    variable_value: float = "",
    mean_time: float = 0.0,
    std_deviation: float = 0.0

@dataclass
class BenchmarkCollectionData:
    name: str = "",
    variable: list[float] = field(default_factory=lambda: [])
    mean: list[float] = field(default_factory=lambda: [])
    std_deviation: list[float] = field(default_factory=lambda: [])

@dataclass
class BenchmarkCollection:
    name: str = "",
    variable_name: str = "",
    data: dict[str, BenchmarkCollectionData] = field(default_factory=lambda: {})

@dataclass
class BenchmarkCase:
    name: str = "",
    benchmark_runs: list[BenchmarkRun] = field(default_factory=lambda: [])

if len(sys.argv) != 3:
    print("Invalid amount of argument")
    exit(-1)

test_file = sys.argv[1]
output_dir = sys.argv[2]

catch2_xml_output = subprocess.run([test_file, "--reporter=XML"], stdout=subprocess.PIPE).stdout
xml_root = ET.XML(catch2_xml_output)

benchmarks: list[BenchmarkCase] = []

for xml_benchmark_case in xml_root.findall("TestCase"):
    benchmark_case = BenchmarkCase()
    benchmark_case.name = xml_benchmark_case.get("name")

    for xml_benchmark_run in xml_benchmark_case.findall("BenchmarkResults"):
        benchmark_run = BenchmarkRun()
        raw_name = xml_benchmark_run.get("name")
        tag = re.findall(r"\[(.*)\]", raw_name)
        if len(tag) >= 1:
            benchmark_run.tag = tag[0].strip()
            raw_name = re.sub(r"\[(.*)\]", "", raw_name, count=1)

        variable_start = raw_name.rfind(" - ")
        if variable_start != -1:
            benchmark_run.name = raw_name[0:variable_start].strip()
            raw_variable_name = raw_name[variable_start + len(" - "):]
            variable_end = raw_variable_name.find("=")
            if variable_end != -1:
                benchmark_run.variable_name = raw_variable_name[0:variable_end].strip()
                benchmark_run.variable_value = float(raw_variable_name[variable_end + 1:].strip())
            else:
                print(f"Warning: benchmark variable {raw_variable_name} has no value")
        else:
            benchmark_run.name = raw_name.strip()

        benchmark_run.mean_time = float(xml_benchmark_run.find("mean").get("value"))
        benchmark_run.std_deviation = float(xml_benchmark_run.find("standardDeviation").get("value"))

        benchmark_case.benchmark_runs.append(benchmark_run)
    benchmarks.append(benchmark_case)


for benchmark in benchmarks:
    benchmark_collections: dict[str, BenchmarkCollection] = {}
    for run in benchmark.benchmark_runs:
        collection = benchmark_collections.get(run.tag, BenchmarkCollection())
        collection.name = run.tag
        collection.variable_name = run.variable_name
        data = collection.data.get(run.name, BenchmarkCollectionData())
        data.name = run.name
        data.variable.append(run.variable_value)
        data.mean.append(run.mean_time)
        data.std_deviation.append(run.std_deviation)
        collection.data[run.name] = data
        benchmark_collections[run.tag] = collection

    for collection_name, collection in benchmark_collections.items():
        plt.figure()
        plt.title(collection_name)
        plt.grid()
        plt.xlabel(collection.variable_name)
        plt.ylabel("time[ns]")
        for name, data in collection.data.items():
            plt.errorbar(data.variable, data.mean, yerr=data.std_deviation, fmt="o", label=name)
        plt.legend()
        output_path = output_dir + "/" + collection_name + ".png"
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        plt.savefig(output_path)
