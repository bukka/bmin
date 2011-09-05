// Quine McCluskey algorithm JavaScript file

// get translated text
function tr(type) {
    if (window.bmin && window.bmin.translations && window.bmin.translations[type])
        return window.bmin.translations[type];
    else
        return type;
}

// combinations container
function Combinations() {
    this.data = (window.bmin && window.bmin.combinations)? window.bmin.combinations: [];
    this.table = document.getElementById("finding_primes");
    this.iter = 0;
}

// reset iterator
Combinations.prototype.reset = function () {
    this.iter = 0;
};


// whether combination object is correctly set
Combinations.prototype.ok = function () {
    return (typeof this.table != "undefined" && this.data.length > 0);
};

// set activity for the one implicant in the table
Combinations.prototype.active = function (comb, isActive, isNext) {
    var mTerm, bTerm, visibility, aColor, nColor;
    if (isNext) {
        visibility = "visible";
        aColor = "#f30";
        nColor = "#000";
    }
    else {
        visibility = "hidden";
        aColor = "#000";
        nColor = "#f30";
    }

    for (var type in comb) {
        mTerm = document.getElementById("m_" + comb[type]);
        bTerm = document.getElementById("b_" + comb[type]);
        if (mTerm && bTerm) {
            if (isActive) {
                if (type == "combined")
                    bTerm.style.visibility = mTerm.style.visibility = visibility;
                bTerm.style.color = mTerm.style.color = aColor;
            }
            else {
                bTerm.style.color = mTerm.style.color = nColor;
            }
        }
    }
};

// get next combination
Combinations.prototype.next = function (btn) {
    if (this.iter > 0)
        this.active(this.data[this.iter - 1], false, true);
    else
        btn.previousSibling.disabled = false;
    if (this.iter <= this.data.length) {
        this.active(this.data[this.iter++], true, true);
        if (this.iter > this.data.length)
            btn.disabled = true;
    }
};

// whether the iterator is not at the end of the container
Combinations.prototype.hasNext = function () {
    return (this.iter < this.data.length);
};

// get previous combination
Combinations.prototype.prev = function (btn) {
    if (this.iter <= this.data.length)
        this.active(this.data[this.iter - 1], true, false);
    else
        btn.nextSibling.disabled = false;
    if (this.iter > 0) {
        this.active(this.data[--this.iter - 1], false, false);
        if (this.iter == 0)
            btn.disabled = true;
    }
};

// whether the iterator is not at the beginning of the container
Combinations.prototype.hasPrev = function () {
    return (this.iter > 0);
};

// set visibility for the implicants in the table
Combinations.prototype.setAllImpls = function (visible) {
    var style = visible? "visible": "hidden";
    var divs = this.table.getElementsByTagName("div");
    for (var i = 0; i < divs.length; i++) {
        if (divs[i].className == "sim")
            divs[i].style.visibility = style;
        divs[i].style.color = "#000";
    }
};

// hide all implicants (before simulation)
Combinations.prototype.hideAll = function() {
    this.setAllImpls(false);
};

// show all implicants (stopping simulation)
Combinations.prototype.showAll = function() {
    this.setAllImpls(true);
};

// create combinations object
var combinations = new Combinations();


// remove button
function fnRemoveButton(parent, id) {
    var btn = document.getElementById(id);
    if (btn)
        parent.removeChild(btn);
}

// start simulation
function fnSimulate() {
    this.value = tr("stopSimulation");
    this.onclick = fnStopSimulation;

    combinations.hideAll();
    combinations.reset();

    // create prev button
    var prevBtn = document.createElement("input");
    prevBtn.type = "button";
    prevBtn.id = "fpi-prev";
    prevBtn.value = tr("prevStep");
    prevBtn.disabled = true;
    prevBtn.onclick = function () {
        combinations.prev(this);
    };
    this.parentNode.appendChild(prevBtn);

    // create next button
    var nextBtn = document.createElement("input");
    nextBtn.type = "button";
    nextBtn.id = "fpi-next";
    nextBtn.value = tr("nextStep");
    nextBtn.onclick = function () {
        combinations.next(this);
    };
    this.parentNode.appendChild(nextBtn);
}

// stop (finish) simulation
function fnStopSimulation() {
    this.value = tr("simulate");
    this.onclick = fnSimulate;

    combinations.showAll();

    var parent = this.parentNode;
    // remove Prev button
    fnRemoveButton(parent, "fpi-prev");
    // remove Next button
    fnRemoveButton(parent, "fpi-next");
}

// get similation button and if exists then initialize the script
var simBtn = document.getElementById("fpi-simulate");
if (simBtn) {
    // check if the combinations object is correctly inicialized
    if (combinations.ok())
        fnStopSimulation.call(simBtn);
    else
        simBtn.disabled = true;
}
