const conversion_factor = 1.95583;

// Function to update the display values for netto and MwSt
function Calculate_MwSt() {
    let mwst_rate_str = document.getElementById('mwst').value.replace(',', '.');
    let mwst_rate = parseFloat(mwst_rate_str) / 100;

    let euro_value_str = document.getElementById('euro').value.replace(',', '.');
    let euro_value = parseFloat(euro_value_str);
    
    if (!isNaN(euro_value) && !isNaN(mwst_rate)) {
        let euro_netto = euro_value / (1 + mwst_rate);
        let euro_mwst = euro_value - euro_netto;

        let dm_value = euro_value * conversion_factor;
        let dm_netto = dm_value / (1 + mwst_rate);
        let dm_mwst = dm_value - dm_netto;

        document.getElementById('euro_netto').innerText = euro_netto.toFixed(2).replace('.', ',');
        document.getElementById('euro_mwst').innerText = euro_mwst.toFixed(2).replace('.', ',');
        document.getElementById('dm_netto').innerText = dm_netto.toFixed(2).replace('.', ',');
        document.getElementById('dm_mwst').innerText = dm_mwst.toFixed(2).replace('.', ',');
    }
}

// Function to calculate DM when Euro input changes
function Calculate_DM() {
    let euro_value_str = document.getElementById('euro').value.replace(',', '.');
    let euro_value = parseFloat(euro_value_str);
    if (!isNaN(euro_value)) {
        let dm_value = euro_value * conversion_factor;
        document.getElementById('dm').value = dm_value.toFixed(2).replace('.', ',');
        Calculate_MwSt();
    }
}

// Function to calculate Euro when DM input changes
function Calculate_Euro(event) {
    let dm_value_str = document.getElementById('dm').value.replace(',', '.');
    let dm_value = parseFloat(dm_value_str);
    if (!isNaN(dm_value)) {
        let euro_value = dm_value / conversion_factor;
        document.getElementById('euro').value = euro_value.toFixed(2).replace('.', ',');
        Calculate_MwSt();
    }
}
