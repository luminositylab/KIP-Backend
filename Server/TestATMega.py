from ATMegaCOM import ATMegaCOM

com = ATMegaCOM()

for i in range(100):
    com.update_robot(100,100,100)

print('Failed attempts:', com.failed_sends)
