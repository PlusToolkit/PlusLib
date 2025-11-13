// Theme toggle functionality for PlusLib documentation
(function() {
    'use strict';

    // Get saved theme preference or default to light
    function getTheme() {
        return localStorage.getItem('pluslib-theme') || 'light';
    }

    // Save theme preference
    function setTheme(theme) {
        localStorage.setItem('pluslib-theme', theme);
        document.documentElement.setAttribute('data-theme', theme);
        updateToggleButton(theme);
    }

    // Update button text based on current theme
    function updateToggleButton(theme) {
        const button = document.getElementById('theme-toggle');
        if (button) {
            if (theme === 'dark') {
                button.innerHTML = '☀️ Light Mode';
                button.setAttribute('aria-label', 'Switch to light mode');
            } else {
                button.innerHTML = '🌙 Dark Mode';
                button.setAttribute('aria-label', 'Switch to dark mode');
            }
        }
    }

    // Toggle between light and dark themes
    function toggleTheme() {
        const currentTheme = getTheme();
        const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
        setTheme(newTheme);
    }

    // Create and insert the theme toggle button
    function createToggleButton() {
        const button = document.createElement('button');
        button.id = 'theme-toggle';
        button.className = 'theme-toggle';
        button.onclick = toggleTheme;
        
        document.body.appendChild(button);
        
        // Set initial theme
        const initialTheme = getTheme();
        setTheme(initialTheme);
    }

    // Initialize when DOM is ready
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', createToggleButton);
    } else {
        createToggleButton();
    }

    // Detect system theme preference if no saved preference
    if (!localStorage.getItem('pluslib-theme')) {
        if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
            setTheme('dark');
        }
    }

    // Listen for system theme changes
    if (window.matchMedia) {
        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', e => {
            if (!localStorage.getItem('pluslib-theme')) {
                setTheme(e.matches ? 'dark' : 'light');
            }
        });
    }
})();
