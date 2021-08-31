# check percentage of available DAOD events
# inputs
# yields file where 3rd column is number of DxAOD input events made using count_Nentry_SumOfWeight.py and countNumberInDxAOD=False
#
# Note - will not work for duplicated samples e.g. AFII or samples with 2 s-tags 361402,361305,361420
#
yields_file_name = 'yields.13TeV_DxAOD_sorted.txt'

#
# AMI DxAOD entries made using FrameworkExe/scripts/getAMIInfo.py
in_file_name = 'dslist_NevtDxAOD.txt'

# 
# output dsid yield_ratio dsname
out_file_name = 'dslist_NevtDxAOD_yield.txt'

try:
    yields_file = open(yields_file_name, 'r')
except IOError:
    print 'could not open', yields_file_name

try:
    in_file = open(in_file_name, 'r')
except IOError:
    print 'could not open', in_file_name

out_file = open(out_file_name, 'w')
#
yields_arr=[]
for yields in yields_file :
    p = yields.split()
    yields_arr.append([p[0],p[2],p[4]])

daod_arr=[]
for lines in in_file:
    p = lines.split()
    daod_arr.append([p[0],p[1],p[2]])

nomatch_arr=[] # to avoid listing twice
for daod in daod_arr:
    #  print '', daod[0]
    match=False
    for yields in yields_arr:
        #print yields[0]
        if yields[0]==daod[0]:
            match=True
            #print 'match',daod[0],daod[1]
            if daod[1]=='0':
                print 'yields[0]',yields[0],'daod[0]',daod[0],yields[1],daod[1] 
                continue
            rat=float(yields[1])/float(daod[1])
            out_file.write(yields[0]+"\t\t"+str(rat)+"\t\t"+yields[2]+"\n") 
    if not match:
        if daod[0] not in nomatch_arr: 
            nomatch_arr.append(daod[0])
            print 'No match found for dataset ', daod[0], daod[2]
        

yields_file.close()
out_file.close() 
in_file.close()


