(() => {
    const themeSwitcher = document.getElementById('theme-switcher');
    (function () {
        let theme = localStorage.getItem('theme');
        console.log(theme);
        if (theme) {
            themeSwitcher.checked = theme == 'dark';
            document.body.classList.add(theme);
        } else {
            document.body.classList.add('light');
        }
    })();
    themeSwitcher.addEventListener('click', (e) => {
        if (e.target.checked) {
            document.body.classList.add('dark');
            document.body.classList.remove('light');
            localStorage.setItem('theme', 'dark');
        } else {
            document.body.classList.add('light');
            document.body.classList.remove('dark');
            localStorage.setItem('theme', 'light');
        }
    });
})();