# Time synchronization for programmable matter based modular robots

*Improving of Modular Robot Time Protocol (MRTP)*

authors:
  * [Bogdan Gorelkin](https://b.gorelkin.me)  <bogdan@gorelkin.me>
  * Hassan Hijazi <hasnhijazi98@gmail.com>

supervisor:
  * [Abdallah Makhoul](https://www.femto-st.fr/en/femto-people/amakhoul) <abdallah.makhoul@univ-fcomte.fr>

curator:
  * Jad Bassil <jad-bassil@hotmail.com>

project relaized in [VisibleSim](https://github.com/VisibleSim/VisibleSim)

---
## Video demonstration 

[![Watch the video](https://b.gorelkin.me/images/MRTP_link_to_youtube.png)](https://youtu.be/x4lbToZrboo)
## Introduction
Modular robots (MR), or modular self-reconfiguring robotic systems, are autonomous systems with variable morphology. These systems are composed of independent connected elements called modules or particles, whose connections to one another form the overall shape of the system. Beyond sensing, processing and communication capabilities, a modular robot includes actuation and motion capabilities that allow it to reconfigure its shape by rearranging connections between modules.

<div align="center">
<img src="https://user-images.githubusercontent.com/74824667/110410618-7236c900-8089-11eb-9e35-712af6e0ef74.png"></br>
</div>

The accurate and efficient operation of many applications and protocols in MR require synchronized notion of time. Unfortunately, built-in hardware clocks of modules are not sufficient alone to fulfill this requirement due to the large number of modules and connections. By means of time synchronization, each module establishes a logical clock whose value at any time represents the network-wide global time. However, this establishment is not straightforward and requires coping with several aspects such as environmental dynamics, various error sources arising from communication, topological changes and power, memory and computation constraints of the modules. These aspects make time synchronization challenging.

The main objective of this project is to propose and implement a new synchronization protocol for modular robots. The idea is to propose an algorithm based on flooding and a local technique that uses Bayesian estimation for reducing the uncertainty error associated with delay and clock inaccuracy.


As result of this work is to compare the synchronization speed of modular robots using two algorithms. In the first case, we are getting a graph of the synchronization speed using the algorithm of Modular Robot Time Protocol (MRTP)<sup id="a1">[1](#f1)</sup>. In the second case, we are trying to improve MRTP algorithm using Simple Algorithm for Improving Time Synchronization in Wireless Sensor Networks<sup id="a2">[2](#f2)</sup>. </br>Result of this work is to compare those two graphs and based on obtained result make a conclusion which one is better.

1.   <b id="f1">Naz A. et al. A time synchronization protocol for modular robots //2016 24th Euromicro International Conference on Parallel, Distributed, and NetworkBased Processing (PDP). – IEEE, 2016. – С. 109-118.</b> 
2.   <b id="f2">Gao Q., Blow K. J., Holding D. J. Simple algorithm for improving time synchronisation in wireless sensor networks //Electronics Letters. – 2004. – Т. 40. – №. 14. – С. 889-891.</b>



