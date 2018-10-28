import numpy as np

def generate_process(beta,process_count,filename):
    '''
    This function generate the process arrival time and burst time
    by sampling from the exponential distribution.

    USAGE:
        INPUT:
            beta            : the parameter for the distribution
            process_count   : the number of process to generate for
            fielname        : where to save the file
        OUTPUT:
            filename.csv    : a file containing the arrival time and
                                cpu burst time for each of the
                                process in separate line.
        Enjoy now.
    '''
    times=np.round(np.random.exponential(scale=beta,
                                        size=(process_count,2)))
    #Saving the processes in text file
    np.savetxt(filename,times,delimiter=',',fmt="%i")

if __name__=='__main__':
    beta=10
    process_count=20
    filename='processes.csv'

    generate_process(beta,process_count,filename)
