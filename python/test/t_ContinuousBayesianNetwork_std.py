#!/usr/bin/env python


import openturns as ot
import otagrum
import pyagrum as gum
import math as m

proto = "A->B->C->D;E->A->C<-E"
bn = gum.BayesNet.fastPrototype(proto)
print("      proto : ", proto)
print("         BN : ", bn)

ndag = otagrum.NamedDAG(bn)
print("       size : ", ndag.getSize())
print("       desc : ", ndag.getDescription())
print("      nodes : ", ndag.getTopologicalOrder())
for nod in ndag.getTopologicalOrder():
    print(" parents(", nod, ") : ", ndag.getParents(nod))
    print("children(", nod, ") : ", ndag.getChildren(nod))

if False:
    order = ndag.getTopologicalOrder()
    marginals = [ot.Uniform(0.0, 1.0) for i in range(order.getSize())]
    copulas = list()
    for i in range(order.getSize()):
        d = 1 + ndag.getParents(i).getSize()
        print("i=", i, ", d=", d)
        if d == 1:
            copulas.append(ot.IndependentCopula(1))
        else:
            R = ot.CorrelationMatrix(d)
            for i in range(d):
                for j in range(i):
                    R[i, j] = 0.5 / d
            copulas.append(ot.Student(5.0, [0.0] * d, [1.0] * d, R).getCopula())

    cbn = otagrum.ContinuousBayesianNetwork(ndag, marginals, copulas)
    print("cbn=", cbn)
    print("cbn pdf=", cbn.computePDF([0.5] * d))
    print("cbn realization=", cbn.getRealization())
    size = 300
    sampleLearn = cbn.getSample(size)
    sample = cbn.getSample(size)

    sampleLearn.exportToCSVFile("samplelearn.csv", ",")
    sample.exportToCSVFile("sample.csv", ",")

    print("cbn sample=", sample)
    logL = 0.0
    pdfSample = cbn.computePDF(sample)
    pdfSample.exportToCSVFile("pdfSample.csv", ",")
    for i in range(size):
        logL += m.log(pdfSample(i, 0))
        logL /= size
    print("log-l=", logL)
    copula = ot.NormalCopulaFactory().buildAsNormalCopula(sampleLearn)
    print("copula=", copula)
    print("log-l=", copula.computeLogPDF(sample).computeMean()[0])
    gr = ot.Graph()
    pairs = ot.Pairs(sample)
    pairs.setPointStyle("dot")
    gr.add(pairs)
    import openturns.viewer as otv

    view = otv.View(gr, (800, 820), square_axes=True)
    view.save("pairs.png")
    view.close()
    gr = ot.Graph()
    pairs = ot.Pairs(copula.getSample(size))
    pairs.setPointStyle("dot")
    gr.add(pairs)
    view = otv.View(gr, (800, 820), square_axes=True)
    view.save("pairsCopula.png")
    view.close()
