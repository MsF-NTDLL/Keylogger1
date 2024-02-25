# Keylogger
A keylogger written in C that has evasion in mind


> 🚫 Disclaimer: Use of this project is for Educational purposes only. Using it on unauthorised machines is strictly forbidden. If somebody is found using it for illegal/malicious intent, we are not to be held responsible.


### Current features
- Logging keys via NtGetRawInputData (**currently being rewritten**)
- IAT obfuscation via API hashing

### To-do

- Delay execution between WinAPI calls
- Disable ETW
- Create a C2 which will process the data
- Data exfil through:
  - Reddit

### References
All references will be included in the Resources directory, alongside other resources.
