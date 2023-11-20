import subprocess

def get_answers(output_dig_str,dns):
    try:
        
        if dns == 1:
            x = output_dig_str.index(";; ANSWER SECTION:")
        else:
            x = output_dig_str.index("Answer")
        
        return x
        

        

    except Exception as e:
        print("An error occurred:", e)
        return []

def address_test(index):
    try:
        
        process_dns = subprocess.Popen(test_cases[index], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        output_dns_str, _ = process_dns.communicate()
        
        process_dig = subprocess.Popen(test_cases[index+1], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        output_dig_str, _ = process_dig.communicate()

        output_dns_split = output_dns_str.split()
        line_index = get_answers(output_dns_split,0)
        answer_dns_split = output_dns_split[line_index+6].split(',')

        
        output_dig_split = output_dig_str.split('\n')
        line_index = get_answers(output_dig_split,1)
        answer_split = output_dig_split[line_index+1].split() 
        
        print("Test", i+1)
        print("Expected :", answer_split[-1])
        print("Got :     ",answer_dns_split[-1])
      
        if answer_dns_split[-1] == answer_split[-1]:
            print("PASSED")
            ok = 1
        else:
            print("FAILED")
            ok = 0

        print("------------------------")

    except Exception as e:
        print("An error occurred:", e)

    return ok


def Qtype_test(index):
    try:
        
        process_dns = subprocess.Popen(test_cases[index], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        output_dns_str, _ = process_dns.communicate()
        
        process_dig = subprocess.Popen(test_cases[index+1], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        output_dig_str, _ = process_dig.communicate()

        output_dns_split = output_dns_str.split()
        line_index = get_answers(output_dns_split,0)
        answer_dns_split = output_dns_split[line_index+4].split(',')

        
        output_dig_split = output_dig_str.split('\n')
        line_index = get_answers(output_dig_split,1)
        answer_split = output_dig_split[line_index+1].split() 
        
        print("Test", i+1)
        print("Expected :", answer_split[-2])
        print("Got :     ",answer_dns_split[0])

        if answer_dns_split[0] == answer_split[-2]:
            print("PASSED")
            ok = 1
        else:
            print("FAILED")
            ok = 0

        print("------------------------")

    except Exception as e:
        print("An error occurred:", e)

    return ok

passed = 0
failed = 0
test_count = 10
i = 0
index = 0
print("---------------------------------------------")
print("ADDRESS TESTS:")

while i < 5:
    #[command_dns,command_dig]
    test_cases = [["./dns", "-s", "kazi.fit.vutbr.cz", "147.229.9.26", "-r", "-x"],["dig", "@kazi.fit.vutbr.cz", "-x", "147.229.9.26"],
                  ["./dns", "-s", "kazi.fit.vutbr.cz", "www.fit.vut.cz", "-r"],["dig", "@kazi.fit.vutbr.cz", "www.fit.vut.cz"],
                  ["./dns", "-s", "kazi.fit.vutbr.cz", "github.com", "-r"],["dig", "@kazi.fit.vutbr.cz", "github.com"],
                  ["./dns", "-s", "one.one.one.one", "google.com", "-r"],["dig", "@one.one.one.one", "google.com"],
                  ["./dns", "-r", "-s", "one.one.one.one", "142.251.37.100", "-x"], ["dig","@one.one.one.one","-x","142.251.37.100"]]
    
    passed = passed + address_test(index)
    index = index + 2
    i = i+1

print("---------------------------------------------")
print("QTYPE TESTS:")
i = 0
index = 0
while i < 5:
    
    passed = passed + Qtype_test(index)
    index = index + 2
    i = i+1

print("---------------------------------------------")
print("Passed: ",passed)
print("Failed:",test_count - passed)