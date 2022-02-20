from random import randrange, choice
import sys
import numpy as np
import argparse

def write_test(infile, outfile, n, q, plusminus1):
    print('Creating test...')
    if plusminus1:
        diffs = np.array([-1,1])[np.random.randint(2, size=n-1)]
        nums = np.cumsum(diffs)
    else:
        nums = np.random.randint(-1e6, +1e6, size=n)
    print('Created example.')

    infile.write('\n'.join([
        str(n),
        ' '.join(map(str, nums)),
        str(q)
    ]))
    infile.write('\n')

    print('Wrote input file')
    queries = np.random.randint(n, size=(q,2))
    queries = np.sort(queries, axis=-1)

    for t in range(q):
        l, r = randrange(n), randrange(n)
        if l > r:
            l, r = r, l
        ans = min(nums[l:r+1])
        infile.write(' '.join([str(l), str(r)]) + '\n')
        outfile.writelines(str(ans) + '\n')

    print('Wrote output file')

if __name__ == '__main__':
    parser = argparse.ArgumentParser('create rmq test')
    parser.add_argument('-in', '--input', type=str, help='file where to write input for test', required=True)
    parser.add_argument('-out', '--output', type=str, help='file where to write output for test', required=True)
    parser.add_argument('-n', '--number', type=int, help='how many elements should the array have', required=True)
    parser.add_argument('-q', '--queries', type=int, help='how many elements should the array have', required=True)
    parser.add_argument('--plusminus1', action='store_true', help='all elements being one apart from each other')
    args = parser.parse_args()

    print(args.plusminus1)

    with open(args.input, mode='wt') as inf:
        with open(args.output, mode='wt') as outf:
            write_test(inf, outf, args.number, args.queries, args.plusminus1)
