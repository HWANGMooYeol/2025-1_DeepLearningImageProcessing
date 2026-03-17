import numpy as np

# input/output data (w/ bias)
X = np.array([
    [0,0,1],
    [0,1,1],
    [1,1,1],
    [1,0,1]])
Y = np.array([[0],[1],[0],[1]])

print(X)
print(X.shape)
print(Y)
print(Y.shape)

# MPL structure & initialization
W0 = 2*np.random.randn(3,4)-1
W1 = 2*np.random.randn(4,1)-1

# training
eta = 1; # learning rate
iterNo = 1000
for i in range(0, iterNo):
    # forward direction
    S0 = np.matmul(X, W0)                                   # S0 = X*W0;
    L1 = np.divide(1,(1+np.exp(-S0)))                       # L1 = 1./(1+exp(-S0));
    S1 = np.matmul(L1, W1)                                  # S1 = L1*W1;
    Yh = np.divide(1,(1+np.exp(-S1)))                       # Yh = 1./(1+exp(-S1));
    # error backpropagation
    dE_dS1 = np.multiply(np.multiply(-(Y-Yh), Yh), 1-Yh)    # dE_dS1 = -(Y-Yh).*Yh.*(1-Yh)
    dE_dL1 = np.matmul(dE_dS1, np.transpose(W1)) 
    dE_dS0 = np.multiply(np.multiply(dE_dL1,L1),1-L1)
    # gradient descent algorithm
    dE_dW1 = np.matmul(np.transpose(L1), dE_dS1)
    dE_dW0 = np.matmul(np.transpose(X), dE_dS0)

    #ADD YOUR CODE HERE 
    #ADD YOUR CODE HERE 
    #ADD YOUR CODE HERE 


    W1 = W1 - eta*dE_dW1                                    # W1 = W1-eta*dE_dW1
    W0 = W0 - eta*dE_dW0                                    # W0 = W0 - eta*dE_dW0
    if (not(i%100)):
        print("iter = {0}, Yh = {1}\n".format(i, Yh.tolist()))
